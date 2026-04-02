#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include "PlantData.h"
/**
 * @class SerialHandler
 * @brief Klasa obsługująca komunikację przez port szeregowy (RS-232/USB).
 * * Odpowiada za nawiązanie połączenia z mikrokontrolerem Arduino, 
 * buforowanie przychodzących bajtów i składanie ich w kompletne linie tekstu.
 */
class SerialHandler : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy SerialHandler.
     * @param parent Wskaźnik na obiekt nadrzędny Qt.
     */
    explicit SerialHandler(PlantData *model, QObject *parent = nullptr);

    /**
     * @brief Otwiera port szeregowy o podanej nazwie.
     * @param portName Nazwa portu (np. "ttyACM0" na Linux lub "COM3" na Windows).
     * @return true jeśli udało się otworzyć port, false w przeciwnym razie.
     */
    bool openPort(const QString &portName);

signals:
    void dataUpdated();

private:
    void parseLine(const QString &line); 
    PlantData *  m_internalData;        

private slots:
    /**
     * @brief Slot obsługujący zdarzenie gotowości danych do odczytu (readyRead).
     * * Odczytuje surowe bajty z portu, dodaje je do bufora i sprawdza, 
     * czy w buforze znajduje się znak końca linii (\n).
     */
    void readData();

private:
    QSerialPort *serial; ///< Obiekt niskopoziomowej obsługi portu Qt.
    QByteArray buffer;   ///< Bufor przechowujący niekompletne fragmenty odebranych danych.
};

#endif