#include "MainWindow.h"
#include <QVBoxLayout>
#include <QStatusBar>

MainWindow::MainWindow(PlantData *data, QWidget *parent) 
    : QMainWindow(parent), plantData(data) {
    

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);

    setupDashboard(); 
    setupStats();   

    serialManager = new SerialHandler(plantData, this);

    connect(serialManager, &SerialHandler::dataUpdated, this, &MainWindow::updateDisplay);

    if (!serialManager->openPort("ttyACM0")) {
    
        statusBar()->showMessage("BŁĄD: Brak połączenia z ttyACM0 (Arduino)", 5000);
        statusBar()->setStyleSheet("color: red; font-weight: bold;");
    } else {
        statusBar()->showMessage("Połączono z systemem monitoringu roślin.", 3000);
    }
    this->setStyleSheet("QMainWindow { background-color: #121212; } "
                        "QTabBar::tab { background: #2D2D2D; color: white; padding: 10px; } "
                        "QTabBar::tab:selected { background: #39FF14; color: black; }");
}

void MainWindow::setupDashboard() {

    m_tempLCD = new QLCDNumber(5);
    m_tempLCD->setSegmentStyle(QLCDNumber::Flat);
    m_presLCD = new QLCDNumber(5);
    m_presLCD->setSegmentStyle(QLCDNumber::Flat);
    m_luxLCD = new QLCDNumber(5);
    m_luxLCD->setSegmentStyle(QLCDNumber::Flat);
    m_soilLCD = new QLCDNumber(5);
    m_soilLCD->setSegmentStyle(QLCDNumber::Flat);

    m_sunIcon = new QLabel;
    m_sunIcon->setPixmap(QPixmap(":/icons/sun.png").scaled(64, 64, Qt::KeepAspectRatio));
    m_waterIcon = new QLabel;
    m_waterIcon->setPixmap(QPixmap(":/icons/water.png").scaled(64, 64, Qt::KeepAspectRatio));
    m_thermometerIcon = new QLabel;
    m_thermometerIcon->setPixmap(QPixmap(":/icons/thermometer.png").scaled(64, 64, Qt::KeepAspectRatio));
    m_pressureIcon = new QLabel;
    m_pressureIcon->setPixmap(QPixmap(":/icons/pressure.png").scaled(64, 64, Qt::KeepAspectRatio));
    
    m_plantWidget = new QLabel;
    m_plantWidget->setPixmap(QPixmap(":/images/plant_ok.png").scaled(256, 256, Qt::KeepAspectRatio));
    m_plantWidget->setAlignment(Qt::AlignCenter);

    m_dashboardPage = new QWidget();
    QGridLayout *layout = new QGridLayout(m_dashboardPage);
    //kolumna 1
    layout->addWidget(m_thermometerIcon, 0, 0);
    layout->addWidget(m_pressureIcon,    1, 0);
    layout->addWidget(m_sunIcon,         2, 0);
    layout->addWidget(m_waterIcon,       3, 0);

    //kolumna 2
    layout->addWidget(m_plantWidget, 0, 1, 4, 1); 
    //kolumna 3
    QLabel *tempLabel = new QLabel("TEMPERATURA[°C]:");
    tempLabel->setStyleSheet("font-weight: bold; color: #1e2222;");
    layout->addWidget(tempLabel, 0, 2);
    layout->addWidget(m_tempLCD, 1, 2);

    QLabel *presLabel = new QLabel("CIŚNIENIE[hPa]:");
    presLabel->setStyleSheet("font-weight: bold; color: #1e2222;");
    layout->addWidget(presLabel, 2, 2);
    layout->addWidget(m_presLCD, 3, 2);
    
    QLabel *luxLabel = new QLabel("ŚWIATŁO[lx]:");
    luxLabel->setStyleSheet("font-weight: bold; color: #1e2222;");
    layout->addWidget(luxLabel, 4, 2);
    layout->addWidget(m_luxLCD,  5, 2);
    
    QLabel *soilLabel = new QLabel("GLEBA[%]:");
    soilLabel->setStyleSheet("font-weight: bold; color: #1e2222;");
    layout->addWidget(soilLabel, 6, 2);
    layout->addWidget(m_soilLCD, 7, 2);

    m_tabs->addTab(m_dashboardPage, tr("Dashboard"));


}

void MainWindow::updateDisplay() {
    // Pobieramy dane z modelu
    m_tempLCD->display(plantData->get_Temperature());
    m_presLCD->display(plantData->get_Pressure());
    m_luxLCD->display(plantData->get_Lux());
    m_soilLCD->display(plantData->get_SoilMoisture());

    // Tutaj później wywołamy animacje z GuiEffects, np:
    // GuiEffects::updateSunIntensity(m_sunIcon, m_dataModel->get_Lux());
}

// void MainWindow::updateDisplay() {
 

//     QString info = QString("TEMPERATURA: %1 °C\n"
//                            "CIŚNIENIE:   %2 hPa\n"
//                            "ŚWIATŁO:     %3 lx\n"
//                            "GLEBA:       %4 %")
//                    .arg(plantData->get_Temperature(), 0, 'f', 1) 
//                    .arg(plantData->get_Pressure(), 0, 'f', 1)
//                    .arg(plantData->get_Lux(), 0, 'f', 0)        
//                    .arg(plantData->get_SoilMoisture());

//     dataLabel->setText(info);
    
//     if(plantData->get_SoilMoisture() < 30) {
//         dataLabel->setStyleSheet("font-size: 20px; color: #FF3131; font-family: 'Courier New';");
//     } else {
//         dataLabel->setStyleSheet("font-size: 20px; color: #41103c; font-family: 'Courier New';");
//     }
// }


void MainWindow::setupStats() {
    m_statsPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_statsPage);
    
    QLabel *placeholder = new QLabel("Miejsce na wykresy");
    placeholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholder);
    
    m_tabs->addTab(m_statsPage, tr("Statystyki"));
}