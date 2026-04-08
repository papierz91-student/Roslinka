#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "SerialHandler.h"
#include "PlantData.h"
#include <QLCDNumber>
#include <QTabWidget>

/**
 * @class MainWindow
 * @brief Klasa głównego okna aplikacji monitorującej parametry rośliny.
 * * Klasa odpowiada za inicjalizację interfejsu użytkownika, zarządzanie połączeniem
 * szeregowym przez SerialHandler oraz interpretację (parsowanie) odebranych danych.
 * Pełni rolę pośrednika między warstwą danych (PlantData) a prezentacją wizualną.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param data Wskaźnik na obiekt klasy PlantData przechowujący stan roślinki.
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr).
     */
    MainWindow(PlantData *data, QWidget *parent = nullptr);

public slots:
    /**
     * @brief Slot aktualizujący elementy interfejsu użytkownika.
     * * Pobiera najświeższe dane z obiektu plantData i odświeża wartości 
     * wyświetlane na widgetach QLCDNumber oraz aktualizuje stan ikon.
     */
    void updateDisplay();

private:
    /**
     * @brief Konfiguruje główny panel kontrolny (Dashboard).
     * Tworzy i układa wyświetlacze LCD oraz ikony statusu.
     */
    void setupDashboard();

    /**
     * @brief Konfiguruje kartę statystyk.
     * Przygotowuje widok przeznaczony do analizy danych historycznych.
     */
    void setupStats();

    /** @name Elementy struktury okna */
    ///@{
    QTabWidget *m_tabs;           ///< Kontener kart (Dashboard / Statystyki).
    QWidget    *m_dashboardPage;   ///< Strona główna z aktualnymi odczytami.
    QWidget    *m_statsPage;       ///< Strona z wykresami i statystykami.
    ///@}

    /** @name Wyświetlacze numeryczne */
    ///@{
    QLCDNumber *m_tempLCD;        ///< Wyświetlacz temperatury.
    QLCDNumber *m_presLCD;        ///< Wyświetlacz ciśnienia.
    QLCDNumber *m_luxLCD;         ///< Wyświetlacz natężenia światła.
    QLCDNumber *m_soilLCD;        ///< Wyświetlacz wilgotności gleby.
    ///@}

    /** @name Elementy graficzne i ikony statusu */
    ///@{
    QLabel *m_sunIcon;            ///< Ikona nasłonecznienia.
    QLabel *m_waterIcon;          ///< Ikona nawodnienia (podlewania).
    QLabel *m_thermometerIcon;    ///< Ikona termometru.
    QLabel *m_pressureIcon;       ///< Ikona barometru.
    QLabel *m_plantWidget;        ///< Widget graficzny reprezentujący roślinę.
    QLabel *m_plantAvatar;        ///< Grafika ośliny.
    ///@}

    /** @name Moduły logiczne */
    ///@{
    SerialHandler *serialManager; ///< Menadżer komunikacji przez port szeregowy.
    PlantData *plantData;         ///< Wskaźnik do modelu danych rośliny.
    ///@}
    
};

#endif // MAINWINDOW_H