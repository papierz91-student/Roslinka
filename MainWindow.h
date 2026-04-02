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

  
    QTabWidget *m_tabs;          
    QWidget    *m_dashboardPage;  
    QWidget    *m_statsPage;     

    QLCDNumber *m_tempLCD;     
    QLCDNumber *m_presLCD; 
    QLCDNumber *m_luxLCD;         
    QLCDNumber *m_soilLCD;        
    QLabel *m_sunIcon;           
    QLabel *m_waterIcon;          
    QLabel *m_thermometerIcon;    
    QLabel *m_pressureIcon;       
    QLabel *m_plantWidget;        

    QLabel *m_plantAvatar;


    SerialHandler *serialManager; 
    PlantData *plantData;        
    
};

#endif