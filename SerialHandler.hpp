#ifndef SERIALHANDLER_HPP
#define SERIALHANDLER_HPP

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo> // Potrzebne do skanowania portów
#include <QTimer>                       // Potrzebne do auto-reconnectu
#include "PlantData.hpp"

class SerialHandler : public QObject {
    Q_OBJECT
public:
    explicit SerialHandler(PlantData *model, QObject *parent = nullptr);
    
    // Zmieniamy na public, jeśli MainWindow nadal chciałoby wymusić otwarcie konkretnego portu
    bool openPort(const QString &portName);

signals:
    void dataUpdated();

private slots:
    void readData();
    void attemptConnection();           // Slot wywoływany przez timer

private:
    QSerialPort *serial;
    QTimer *reconnectTimer;             // Licznik czasu dla ponownych prób
    PlantData *m_internalData;
    QByteArray buffer;

    void parseLine(const QString &line);
    bool validateChecksum(const QString &line); // Weryfikacja integralności danych[cite: 1]
};

#endif // SERIALHANDLER_HPP