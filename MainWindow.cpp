#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <algorithm>

// ===================== KARTA (STYL ORYGINALNY) =====================
QWidget* MainWindow::createCard(const QString &title, QLabel *icon, QLabel *value) {
    QWidget *card = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(card);

    QLabel *titleLabel = new QLabel(title.toUpper());
    titleLabel->setObjectName("cardTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #888; font-weight: bold; border: none; background: transparent;");

    // Ważne: value już ma przypisany ObjectName w setupDashboard
    value->setAlignment(Qt::AlignCenter);
    value->setStyleSheet("color: #39FF14; font-weight: bold; border: none; background: transparent;");

    layout->addWidget(titleLabel);
    layout->addWidget(value);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(5);

    card->setStyleSheet(
        "QWidget { background-color: #1E1E1E; border: 1px solid #333; }"
    );

    return card;
}

// ===================== KONSTRUKTOR =====================
MainWindow::MainWindow(PlantData *data, QWidget *parent) 
    : QMainWindow(parent), plantData(data) {

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);

    sunPixmap = QPixmap("../icons/sun.png");
    waterPixmap = QPixmap("../icons/water.png");
    thermoPixmap = QPixmap("../icons/termometer.png");
    pressurePixmap = QPixmap("../icons/pressure.png");
    plantPixmap = QPixmap("../icons/plant_ok.png");

    setupDashboard(); 
    setupStats();   

    serialManager = new SerialHandler(plantData, this);
    connect(serialManager, &SerialHandler::dataUpdated, this, &MainWindow::updateDisplay);

    if (!serialManager->openPort("ttyACM0")) {
        statusBar()->showMessage("BŁĄD: ttyACM0 offline", 5000);
    }

    this->setStyleSheet(
        "QMainWindow { background-color: #0F0F0F; } "
        "QTabBar::tab { background: #2A2A2A; color: #aaa; padding: 10px 25px; } "
        "QTabBar::tab:selected { background: #39FF14; color: black; font-weight: bold; }"
    );
}

// ===================== DASHBOARD =====================
void MainWindow::setupDashboard() {
    m_dashboardPage = new QWidget();
    QGridLayout *layout = new QGridLayout(m_dashboardPage);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(30);

    m_thermometerIcon = new QLabel;
    m_pressureIcon = new QLabel;
    m_sunIcon = new QLabel;
    m_waterIcon = new QLabel;
    m_plantAvatar = new QLabel;

    // PRZYPISANIE OBJECT NAME PRZED DODANIEM DO KARTY
    QLabel *tempValue = new QLabel("--"); tempValue->setObjectName("tempValue");
    QLabel *presValue = new QLabel("--"); presValue->setObjectName("presValue");
    QLabel *luxValue  = new QLabel("--"); luxValue->setObjectName("luxValue");
    QLabel *soilValue = new QLabel("--"); soilValue->setObjectName("soilValue");

    layout->addWidget(m_thermometerIcon, 0, 0, Qt::AlignCenter);
    layout->addWidget(createCard("Temperatura [°C]", nullptr, tempValue), 0, 2);

    layout->addWidget(m_pressureIcon, 1, 0, Qt::AlignCenter);
    layout->addWidget(createCard("Ciśnienie [hPa]", nullptr, presValue), 1, 2);

    layout->addWidget(m_sunIcon, 2, 0, Qt::AlignCenter);
    layout->addWidget(createCard("Światło [lx]", nullptr, luxValue), 2, 2);

    layout->addWidget(m_waterIcon, 3, 0, Qt::AlignCenter);
    layout->addWidget(createCard("Gleba [%]", nullptr, soilValue), 3, 2);

    m_plantAvatar->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_plantAvatar, 0, 1, 4, 1); 

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 4);
    layout->setColumnStretch(2, 3);

    m_tabs->addTab(m_dashboardPage, tr("Dashboard"));
}

// ===================== RESIZE (DYNAMICZNE CZCIONKI) =====================
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // 1. IKONY
    int iconSize = std::max(64, width() / 15); 
    int plantSize = std::max(250, width() / 3);

    auto scale = [&](QPixmap &p, int s) { 
        return p.isNull() ? QPixmap() : p.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation); 
    };

    m_sunIcon->setPixmap(scale(sunPixmap, iconSize));
    m_waterIcon->setPixmap(scale(waterPixmap, iconSize));
    m_thermometerIcon->setPixmap(scale(thermoPixmap, iconSize));
    m_pressureIcon->setPixmap(scale(pressurePixmap, iconSize));
    m_plantAvatar->setPixmap(scale(plantPixmap, plantSize));

    // 2. TEKST - Zmiana przez QFont zamiast setStyleSheet (nie czyści tekstu)
    int valueFontSize = std::max(18, width() / 35);
    int titleFontSize = std::max(9, width() / 100);

    QFont vFont; vFont.setPixelSize(valueFontSize); vFont.setBold(true);
    QFont tFont; tFont.setPixelSize(titleFontSize); tFont.setBold(true);

    // Aktualizacja czcionek dla wartości
    QStringList values = {"tempValue", "presValue", "luxValue", "soilValue"};
    for (const QString &name : values) {
        QLabel *l = findChild<QLabel*>(name);
        if (l) l->setFont(vFont);
    }

    // Aktualizacja czcionek dla tytułów kart
    QList<QLabel*> allLabels = findChildren<QLabel*>("cardTitle");
    for (QLabel* l : allLabels) {
        l->setFont(tFont);
    }
}

// ===================== UPDATE (POKAZYWANIE DANYCH) =====================
void MainWindow::updateDisplay() {
    QLabel *temp = findChild<QLabel*>("tempValue");
    QLabel *pres = findChild<QLabel*>("presValue");
    QLabel *lux  = findChild<QLabel*>("luxValue");
    QLabel *soil = findChild<QLabel*>("soilValue");

    if (temp) temp->setText(QString::number(plantData->get_Temperature(), 'f', 1) + " °C");
    if (pres) pres->setText(QString::number(plantData->get_Pressure(), 'f', 0) + " hPa");
    if (lux)  lux->setText(QString::number(plantData->get_Lux(), 'f', 0) + " lx");
    if (soil) soil->setText(QString::number(plantData->get_SoilMoisture()) + " %");
}

void MainWindow::setupStats() {
    m_statsPage = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_statsPage);
    QLabel *placeholder = new QLabel("Statystyki");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #555; font-size: 20px;");
    layout->addWidget(placeholder);
    m_tabs->addTab(m_statsPage, tr("Statystyki"));
}