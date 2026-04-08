#include "SerialHandler.hpp"
#include <QDebug>

SerialHandler::SerialHandler(PlantData *model, QObject *parent) 
    : QObject(parent), m_internalData(model) {
    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, &SerialHandler::readData);
}

bool SerialHandler::openPort(const QString &portName) {
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Port szeregowy otwarty:" << portName;
        return true;
    } else {
        qDebug() << "Błąd otwarcia portu:" << serial->errorString();
        return false;
    }
}

void SerialHandler::readData() {
    buffer.append(serial->readAll());
    while (buffer.contains('<') && buffer.contains('>')) {
        int start = buffer.indexOf('<');
        int end = buffer.indexOf('>', start);
        
        if (end > start) {
            QString line = QString::fromUtf8(buffer.mid(start, end - start + 1));
            buffer.remove(0, end + 1); 
            
            parseLine(line); 
            emit dataUpdated(); 
        } else {
            break; 
        }
    }
}

void SerialHandler::parseLine(const QString &line) {
    QString cleanLine = line;
    cleanLine.remove('<');
    cleanLine.remove('>');
    QStringList dataList = cleanLine.split(';');

    foreach (const QString &item, dataList) {
        QString trimmedItem = item.trimmed(); 
        
        if (trimmedItem.startsWith("T:")) {
            m_internalData->set_Temperature(trimmedItem.mid(2).toDouble());
        } else if (trimmedItem.startsWith("P:")) {
            m_internalData->set_Pressure(trimmedItem.mid(2).toDouble());
        } else if (trimmedItem.startsWith("L:")) {
            m_internalData->set_Lux(trimmedItem.mid(2).toDouble());
        } else if (trimmedItem.startsWith("S:")) {
            m_internalData->set_SoilMoisture(trimmedItem.mid(2).toInt());
        }
    }
}