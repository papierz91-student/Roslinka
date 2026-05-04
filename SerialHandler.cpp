#include "SerialHandler.hpp"
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

SerialHandler::SerialHandler(PlantData *model, QObject *parent) 
    : QObject(parent), m_internalData(model) {
    serial = new QSerialPort(this);

    reconnectTimer = new QTimer(this);
    connect(reconnectTimer, &QTimer::timeout, this, &SerialHandler::attemptConnection);
    
    connect(serial, &QSerialPort::readyRead, this, &SerialHandler::readData);

    // Poprawiona obsługa błędów (Sytuacje wyjątkowe)
    connect(serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        // Ignorujemy brak błędu i błędy otwarcia (obsłużone w attemptConnection)
        if (error == QSerialPort::NoError || error == QSerialPort::OpenError) return;

        // Reagujemy tylko na faktyczne zerwanie połączenia
        if (error == QSerialPort::ResourceError || error == QSerialPort::PermissionError) {
            if (serial->isOpen()) {
                qDebug() << "Utracono połączenie fizyczne z urządzeniem.";
                serial->close();
            }
            
            if (!reconnectTimer->isActive()) {
                reconnectTimer->start(2000);
            }
        }
    });

    reconnectTimer->start(1000); 
}

void SerialHandler::attemptConnection() {
    // Jeśli port jest już otwarty, nie rób nic
    if (serial->isOpen()) {
        reconnectTimer->stop();
        return;
    }

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        // Detekcja portu (np. Arduino Uno R3[cite: 1])
        if (info.description().contains("Arduino") || info.portName().contains("ttyACM") || info.portName().contains("COM")) {
            
            serial->setPort(info);
            // Ważne: Czyścimy stary stan błędu przed nową próbą
            serial->clearError(); 

            if (openPort(info.portName())) {
                qDebug() << "Połączono pomyślnie z:" << info.portName();
                reconnectTimer->stop();
                return;
            }
        }
    }
}

bool SerialHandler::openPort(const QString &portName) {
    // Parametry zgodne z protokołem komunikacyjnym[cite: 1]
    serial->setBaudRate(QSerialPort::Baud9600); 
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    return serial->open(QIODevice::ReadOnly);
}

// Reszta metod (readData, validateChecksum, parseLine) pozostaje bez zmian

void SerialHandler::readData() {
    buffer.append(serial->readAll());
    while (buffer.contains('<') && buffer.contains('>')) {
        int start = buffer.indexOf('<');
        int end = buffer.indexOf('>', start);
        
        if (end > start) {
            QString line = QString::fromUtf8(buffer.mid(start, end - start + 1));
            buffer.remove(0, end + 1); 
            
            if (validateChecksum(line)) { 
                parseLine(line); 
                emit dataUpdated(); 
            } else {
                qDebug() << "Błąd sumy kontrolnej w ramce:" << line;
            }
        } else {
            break; 
        }
    }
}

bool SerialHandler::validateChecksum(const QString &line) {
    if (!line.contains("CS:")) return false;

    int csPos = line.lastIndexOf("CS:");
    QString dataPart = line.mid(1, csPos - 1); 
    QString receivedCSStr = line.mid(csPos + 3);
    receivedCSStr.remove('>');
    
    int receivedCS = receivedCSStr.toInt();
    int calculatedCS = 0;

    QByteArray bytes = dataPart.toUtf8();
    for (char b : bytes) {
        calculatedCS ^= static_cast<unsigned char>(b);
    }

    return (calculatedCS == receivedCS);
}

void SerialHandler::parseLine(const QString &line) {
    QString cleanLine = line;
    cleanLine.remove('<');
    cleanLine.remove('>');
    QStringList dataList = cleanLine.split(';');

    foreach (const QString &item, dataList) {
        QString trimmedItem = item.trimmed(); 
        if (trimmedItem.startsWith("T:")) m_internalData->set_Temperature(trimmedItem.mid(2).toDouble());
        else if (trimmedItem.startsWith("P:")) m_internalData->set_Pressure(trimmedItem.mid(2).toDouble());
        else if (trimmedItem.startsWith("L:")) m_internalData->set_Lux(trimmedItem.mid(2).toDouble());
        else if (trimmedItem.startsWith("S:")) m_internalData->set_SoilMoisture(trimmedItem.mid(2).toInt());
    }
}