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
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr).
     */
    MainWindow(PlantData *data, QWidget *parent = nullptr);
  

public slots:
    /**
     * @brief Slot wywoływany po odebraniu nowych danych z portu szeregowego.
     * @param data Ciąg znaków odebrany przez SerialHandler.
     */
    void updateDisplay();

private:
    void setupDashboard();
    void setupStats();

    // --- KONTENERY GŁÓWNE (Struktura okna) ---
    QTabWidget *m_tabs;            // Główny przełącznik zakładek (Dashboard / Statystyki)
    QWidget    *m_dashboardPage;   // Strona z roślinką i aktualnymi danymi
    QWidget    *m_statsPage;       // Strona, gdzie w Etapie 4 wstawisz wykresy

    // --- WIDŻETY DANYCH (Prawa strona - Liczniki LCD) ---
    // Używamy QLCDNumber dla "technicznego" wyglądu
    QLCDNumber *m_tempLCD;         // Wyświetlacz temperatury [°C]
    QLCDNumber *m_presLCD;         // Wyświetlacz ciśnienia [hPa]
    QLCDNumber *m_luxLCD;          // Wyświetlacz natężenia światła [lx]
    QLCDNumber *m_soilLCD;          // Wyświetlacz wilgotności gleby [%]

    // --- WIDŻETY WIZUALNE (Lewa strona - Ikony/Grafiki) ---
    // QLabel służy tu jako kontener na obrazki (QPixmap)
    QLabel *m_sunIcon;             // Ikona słońca (będzie zmieniać jasność w GuiEffects)
    QLabel *m_waterIcon;            // Ikona kropli (może pulsować, gdy sucho)
    QLabel *m_thermometerIcon;     // Statyczna lub animowana ikona termometru
    QLabel *m_pressureIcon;        // Ikona barometru/ciśnienia

    // --- CENTRUM (Środek - Serce systemu) ---
    QLabel *m_plantWidget;         // Główna grafika roślinki (nasz "Avatar" stanu rośliny)

    // --- LOGIKA I DANE ---
    SerialHandler *serialManager; ///< Menadżer komunikacji szeregowej.
    PlantData *plantData;        ///< Wskaźnik na obiekt przechowujący dane rośliny.
    
};

#endif