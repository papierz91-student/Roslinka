#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(PlantData *data, QWidget *parent) : QMainWindow(parent), plantData(data) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    dataLabel = new QLabel("OCZEKIWANIE NA DANE...");
    dataLabel->setStyleSheet("font-size: 24px; color: #39FF14; font-family: 'Courier New';");
    
    layout->addWidget(dataLabel);
    setCentralWidget(centralWidget);
    serialManager = new SerialHandler(plantData, this);


    connect(serialManager, &SerialHandler::dataUpdated, this, &MainWindow::updateDisplay);
    if (!serialManager->openPort("ttyACM0")) {
        dataLabel->setText("BŁĄD: Nie można otworzyć portu ttyACM0");
        dataLabel->setStyleSheet("color: red;");
    }
}

void MainWindow::updateDisplay() {
 

    QString info = QString("TEMPERATURA: %1 °C\n"
                           "CIŚNIENIE:   %2 hPa\n"
                           "ŚWIATŁO:     %3 lx\n"
                           "GLEBA:       %4 %")
                   .arg(plantData->get_Temperature(), 0, 'f', 1) 
                   .arg(plantData->get_Pressure(), 0, 'f', 1)
                   .arg(plantData->get_Lux(), 0, 'f', 0)        
                   .arg(plantData->get_SoilMoisture());

    dataLabel->setText(info);
    
    if(plantData->get_SoilMoisture() < 30) {
        dataLabel->setStyleSheet("font-size: 20px; color: #FF3131; font-family: 'Courier New';");
    } else {
        dataLabel->setStyleSheet("font-size: 20px; color: #39FF14; font-family: 'Courier New';");
    }
}


