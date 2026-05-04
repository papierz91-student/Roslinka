#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "SerialHandler.hpp"
#include "PlantData.hpp"
#include <QLCDNumber>
#include <QTabWidget>
#include <QResizeEvent> // ✅ dodane

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

protected:
    void resizeEvent(QResizeEvent *event) override; // ✅ dodane

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

    QLabel* createStyledLabel(const QString &text); // ✅ helper

    /** @name Elementy struktury okna */
    ///@{
    QTabWidget *m_tabs;           
    QWidget    *m_dashboardPage;  
    QWidget    *m_statsPage;      
    ///@}

    /** @name Wyświetlacze numeryczne */
    ///@{
    QLCDNumber *m_tempLCD;        
    QLCDNumber *m_presLCD;        
    QLCDNumber *m_luxLCD;         
    QLCDNumber *m_soilLCD;        
    ///@}

    /** @name Elementy graficzne i ikony statusu */
    ///@{
    QLabel *m_sunIcon;            
    QLabel *m_waterIcon;          
    QLabel *m_thermometerIcon;    
    QLabel *m_pressureIcon;       
    QLabel *m_plantWidget;        
    QLabel *m_plantAvatar;        
    ///@}

    /** @name Moduły logiczne */
    ///@{
    SerialHandler *serialManager; 
    PlantData *plantData;         
    ///@}

    QWidget* createCard(const QString &title, QLabel *icon, QLabel *value);
    QPixmap sunPixmap, waterPixmap, thermoPixmap, pressurePixmap, plantPixmap;
};

#endif // MAINWINDOW_H