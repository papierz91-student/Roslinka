#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "SerialHandler.h"
#include "PlantData.h"

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
    QLabel *dataLabel;            ///< Etykieta wyświetlająca dane w GUI.
    SerialHandler *serialManager; ///< Menadżer komunikacji szeregowej.
    PlantData *plantData;        ///< Wskaźnik na obiekt przechowujący dane rośliny.
    
};

#endif