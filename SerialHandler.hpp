/**
 * @file SerialHandler.hpp
 * @brief Klasa odpowiedzialna za niskopoziomową komunikację szeregową i parsowanie protokołu.
 * @author Michał Papierzański
 * @date 2026-05-05
 */

#ifndef SERIALHANDLER_HPP
#define SERIALHANDLER_HPP

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo> 
#include <QTimer>                       
#include "PlantData.hpp"

/**
 * @class SerialHandler
 * @brief Obsługuje połączenie z mikrokontrolerem przez port szeregowy (RS-232/USB).
 * 
 * Klasa implementuje automatyczne wykrywanie urządzeń (Arduino/COM/ttyACM),
 * asynchroniczny odczyt danych z buforowaniem oraz weryfikację integralności ramek
 * za pomocą sumy kontrolnej XOR. Dane po poprawnym sparsowaniu trafiają bezpośrednio
 * do współdzielonego modelu PlantData.
 */
class SerialHandler : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy SerialHandler.
     * @param model Wskaźnik do obiektu PlantData, który będzie aktualizowany nowymi danymi.
     * @param parent Wskaźnik do obiektu rodzica Qt.
     * 
     * Inicjalizuje port szeregowy, konfiguruje mechanizm reconnectTimer oraz
     * łączy sygnały błędów i gotowości danych.
     */
    explicit SerialHandler(PlantData *model, QObject *parent = nullptr);
  
    /**
     * @brief Konfiguruje i próbuje otworzyć określony port szeregowy.
     * @param portName Nazwa portu (np. "COM3" lub "ttyACM0").
     * @return true jeśli port został pomyślnie otwarty w trybie ReadOnly, false w przeciwnym razie.
     * 
     * Metoda ustawia parametry transmisji: 9600 baud, 8 bitów danych, brak parzystości, 1 bit stopu.
     */
    bool openPort(const QString &portName);

signals:
    /**
     * @brief Sygnał emitowany po pomyślnym odebraniu i sparsowaniu pełnej ramki danych.
     * Informuje interfejs użytkownika o konieczności odświeżenia widoku.
     */
    void dataUpdated();

private slots:
    /**
     * @brief Slot wywoływany, gdy w buforze systemowym portu pojawią się nowe bajty.
     * 
     * Odpowiada za akumulację danych w wewnętrznym buforze (QByteArray) oraz
     * wycinanie kompletnych ramek ograniczonych znakami '<' i '>'.
     */
    void readData();

    /**
     * @brief Realizuje logikę automatycznego wyszukiwania i łączenia z urządzeniem.
     * 
     * Przeszukuje dostępne porty w systemie pod kątem opisów zawierających "Arduino"
     * lub nazw pasujących do standardowych portów szeregowych. Zatrzymuje timer po sukcesie.
     */
    void attemptConnection();     

private:
    QSerialPort *serial;          ///< Obiekt Qt zarządzający fizyczną komunikacją z portem.
    QTimer *reconnectTimer;       ///< Timer odpowiedzialny za cykliczne próby przywrócenia połączenia.
    PlantData *m_internalData;    ///< Wskaźnik do modelu danych (PlantData), gdzie zapisywane są wyniki pomiarów.
    QByteArray buffer;            ///< Bufor przechowujący surowe dane oczekujące na znalezienie znaku końca ramki.

    /**
     * @brief Wyodrębnia wartości liczbowe z tekstowej ramki danych.
     * @param line Oczyszczona ramka danych (bez znaczników początku/końca i sumy kontrolnej).
     * 
     * Obsługuje tokeny: T (Temperatura), P (Ciśnienie), L (Światło), S (Wilgotność gleby).
     */
    void parseLine(const QString &line);

    /**
     * @brief Weryfikuje poprawność danych przy użyciu sumy kontrolnej XOR.
     * @param line Pełna surowa ramka odebrana z portu.
     * @return true jeśli obliczona suma XOR danych zgadza się z wartością po polu "CS:", false w przypadku błędu.
     */
    bool validateChecksum(const QString &line); 
};

#endif // SERIALHANDLER_HPP