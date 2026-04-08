#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include "PlantData.h"

/**
 * @class SerialHandler
 * @brief Klasa obsługująca komunikację przez port szeregowy (RS-232/USB).
 * * Odpowiada za nawiązanie połączenia z mikrokontrolerem (np. Arduino), 
 * buforowanie przychodzących bajtów, składanie ich w kompletne linie tekstu
 * oraz aktualizację modelu danych.
 */
class SerialHandler : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy SerialHandler.
     * @param model Wskaźnik na obiekt PlantData, który będzie aktualizowany danymi z portu.
     * @param parent Wskaźnik na obiekt nadrzędny Qt (zarządzanie pamięcią).
     */
    explicit SerialHandler(PlantData *model, QObject *parent = nullptr);

    /**
     * @brief Konfiguruje i otwiera port szeregowy o podanej nazwie.
     * * Ustawia parametry transmisji (np. BaudRate 9600, 8N1).
     * @param portName Nazwa systemowa portu (np. "ttyACM0" na Linux lub "COM3" na Windows).
     * @return true jeśli udało się poprawnie otworzyć i skonfigurować port, false w przeciwnym razie.
     */
    bool openPort(const QString &portName);

signals:
    /**
     * @brief Sygnał emitowany po pomyślnym sparsowaniu nowej porcji danych.
     * * Informuje inne komponenty (np. MainWindow), że stan obiektu PlantData uległ zmianie.
     */
    void dataUpdated();

private:
    /**
     * @brief Interpretuje surowy ciąg znaków i aktualizuje pola w m_internalData.
     * * Przetwarza ramkę danych (np. format tekstowy lub JSON) na wartości liczbowe.
     * @param line Pełna linia tekstu odebrana z portu szeregowego.
     */
    void parseLine(const QString &line); 

    PlantData *m_internalData;  ///< Wskaźnik do współdzielonego modelu danych rośliny.

private slots:
    /**
     * @brief Slot obsługujący zdarzenie readyRead() z QSerialPort.
     * * Odczytuje surowe bajty z portu, dołącza je do wewnętrznego bufora
     * i wyszukuje znaki końca linii, aby wyodrębnić pełne komunikaty.
     */
    void readData();

private:
    QSerialPort *serial; ///< Obiekt Qt odpowiedzialny za sprzętową obsługę portu.
    QByteArray buffer;   ///< Bufor przechowujący fragmenty danych czekające na przetworzenie.
};

#endif // SERIALHANDLER_H