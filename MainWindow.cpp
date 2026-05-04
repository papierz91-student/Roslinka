#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <QPainter>
#include <QScrollArea>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <algorithm>

// Nagłówki dla wykresów
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

QT_USE_NAMESPACE 

MainWindow::MainWindow(PlantData *data, QWidget *parent) 
    : QMainWindow(parent), plantData(data) {

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);
    
    serialManager = new SerialHandler(plantData, this);
    
    connect(serialManager, &SerialHandler::dataUpdated, this, &MainWindow::updateDisplay);


    QString p = "../icons/";
    sunPixmap.load(p + "sun.png");
    waterPixmap.load(p + "water.png");
    thermoPixmap.load(p + "termometer.png");
    pressurePixmap.load(p + "pressure.png");

    plantPixmap.load(p + "plant_ok.png");
    plantDarkPixmap.load(p + "plant_dark.png");
    plantDryPixmap.load(p + "plant_dry.png");
    plantHotPixmap.load(p + "plant_hot.png");
    plantWetPixmap.load(p + "plant_wet.png");
    plantColdPixmap.load(p + "plant_cold.png");

    setupDashboard(); 
    setupStats();   
    loadHistoryFromCSV();

    // Archiwizacja co 60 sekund
    archiveTimer = new QTimer(this);
    connect(archiveTimer, &QTimer::timeout, this, &MainWindow::saveToArchive);
    archiveTimer->start(60000);

    // Serial Port
    serialManager = new SerialHandler(plantData, this);
    connect(serialManager, &SerialHandler::dataUpdated, this, &MainWindow::updateDisplay);

    if (!serialManager->openPort("ttyACM0")) {
        statusBar()->showMessage("BŁĄD: Port ttyACM0 jest niedostępny", 5000);
    }

    this->setStyleSheet("QMainWindow { background-color: #1A1A1A; } "
                    "QTabWidget::pane { border: none; background: #1A1A1A; } "
                    "QTabBar::tab { background: #282828; color: #BBB; padding: 10px; } "
                    "QTabBar::tab:selected { background: #333; color: #39FF14; }");
}

QWidget* MainWindow::createCard(const QString &title, QLabel *icon, QLabel *value) {
    QWidget *card = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(card);

    QLabel *titleLabel = new QLabel(title.toUpper());
    titleLabel->setObjectName("cardTitle"); // TO JEST KLUCZOWE
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #888; font-weight: bold; border: none; background: transparent;");

    value->setAlignment(Qt::AlignCenter);
    value->setStyleSheet("color: #39FF14; font-weight: bold; border: none; background: transparent;");

    layout->addWidget(titleLabel);
    layout->addWidget(value);
    layout->setContentsMargins(15, 10, 15, 10);
    layout->setSpacing(5);

    card->setStyleSheet("QWidget { background-color: #1E1E1E; border: 1px solid #333; border-radius: 8px; }");
    return card;
}

void MainWindow::setupDashboard() {
    m_dashboardPage = new QWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(m_dashboardPage);
    mainVLayout->setContentsMargins(20, 20, 20, 20);

    // Pasek górny dla przycisku (niezależny od siatki kart)
    QHBoxLayout *topBar = new QHBoxLayout();
    m_langBtn = new QPushButton("PL / EN");
    m_langBtn->setFixedSize(80, 30);
    m_langBtn->setStyleSheet(
        "QPushButton { background: #333; color: #39FF14; border: 1px solid #39FF14; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #444; }"
    );
    connect(m_langBtn, &QPushButton::clicked, this, &MainWindow::toggleLanguage);
    
    topBar->addStretch(); // Pcha przycisk do prawej krawędzi
    topBar->addWidget(m_langBtn);
    mainVLayout->addLayout(topBar);

    // Siatka dla ikon i kart
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(30);

    m_thermometerIcon = new QLabel;
    m_pressureIcon = new QLabel;
    m_sunIcon = new QLabel;
    m_waterIcon = new QLabel;
    m_plantAvatar = new QLabel;

    QLabel *tempValue = new QLabel("--"); tempValue->setObjectName("tempValue");
    QLabel *presValue = new QLabel("--"); presValue->setObjectName("presValue");
    QLabel *luxValue  = new QLabel("--"); luxValue->setObjectName("luxValue");
    QLabel *soilValue = new QLabel("--"); soilValue->setObjectName("soilValue");

    // Rozmieszczenie w siatce
    gridLayout->addWidget(m_thermometerIcon, 0, 0, Qt::AlignCenter);
    gridLayout->addWidget(createCard("TEMPERATURE", nullptr, tempValue), 0, 2);

    gridLayout->addWidget(m_pressureIcon, 1, 0, Qt::AlignCenter);
    gridLayout->addWidget(createCard("PRESSURE", nullptr, presValue), 1, 2);

    gridLayout->addWidget(m_sunIcon, 2, 0, Qt::AlignCenter);
    gridLayout->addWidget(createCard("BRIGHTNESS", nullptr, luxValue), 2, 2);

    gridLayout->addWidget(m_waterIcon, 3, 0, Qt::AlignCenter);
    gridLayout->addWidget(createCard("MOISTURE", nullptr, soilValue), 3, 2);

    m_plantAvatar->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(m_plantAvatar, 0, 1, 4, 1); 

    gridLayout->setColumnStretch(1, 2);
    mainVLayout->addLayout(gridLayout);
    
    m_tabs->addTab(m_dashboardPage, "Main Screen");
}

void MainWindow::setupStats() {
    m_statsPage = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(m_statsPage);

    QScrollArea *scroll = new QScrollArea();
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    
    QStringList names = {"Temperatura [°C]", "Wilgotność [%]", "Światło [lx]", "Ciśnienie [hPa]"};
    QList<QColor> colors = {Qt::red, Qt::cyan, Qt::yellow, Qt::magenta};

    for(int i = 0; i < 4; ++i) {
        m_series[i] = new QLineSeries();
        m_series[i]->setName(names[i]);
        m_series[i]->setPen(QPen(colors[i], 2));

        m_charts[i] = new QChart();
        m_charts[i]->addSeries(m_series[i]);
        m_charts[i]->setTitle(names[i]);
        m_charts[i]->setTitleBrush(QBrush(Qt::white));
        m_charts[i]->setBackgroundVisible(false);
        m_charts[i]->legend()->hide();

        m_axesX[i] = new QDateTimeAxis();
        m_axesX[i]->setFormat("hh:mm");
        m_axesX[i]->setLabelsColor(Qt::white);
        m_charts[i]->addAxis(m_axesX[i], Qt::AlignBottom);
        m_series[i]->attachAxis(m_axesX[i]);

        m_axesY[i] = new QValueAxis();
        m_axesY[i]->setLabelsColor(Qt::white);
        m_charts[i]->addAxis(m_axesY[i], Qt::AlignLeft);
        m_series[i]->attachAxis(m_axesY[i]);

        m_chartViews[i] = new QChartView(m_charts[i]);
        m_chartViews[i]->setRenderHint(QPainter::Antialiasing);
        m_chartViews[i]->setMinimumHeight(350); 
        m_chartViews[i]->setStyleSheet("background: transparent;");
        
        scrollLayout->addWidget(m_chartViews[i]);
        connect(m_series[i], &QLineSeries::hovered, this, &MainWindow::showPointValue);
    }

    scroll->setWidget(scrollContent);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background: #1A1A1A; }");
    mainLayout->addWidget(scroll);

    m_chartScroll = new QScrollBar(Qt::Horizontal);
    m_chartScroll->setStyleSheet("QScrollBar:horizontal { height: 25px; background: #222; } "
                                "QScrollBar::handle:horizontal { background: #39FF14; }");
    mainLayout->addWidget(m_chartScroll);

    connect(m_chartScroll, &QScrollBar::valueChanged, this, [this](int val){
        m_autoScroll = (val >= m_chartScroll->maximum() - 1);
        updateChartsWindow();
    });

    m_tooltipLabel = new QLabel(this);
    m_tooltipLabel->setWindowFlags(Qt::ToolTip);
    m_tooltipLabel->setStyleSheet("background: #333; color: white; padding: 5px; border-radius: 5px;");
    m_tooltipLabel->hide();

    m_clearBtn = new QPushButton("WYCZYŚĆ HISTORIĘ");
    m_clearBtn->setStyleSheet("background: #551111; color: white; padding: 10px;");
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearHistory);
    mainLayout->addWidget(m_clearBtn);

    m_tabs->addTab(m_statsPage, "Statystyki");
}

void MainWindow::updateDisplay() {
    if (!plantData) return;

    double t = plantData->get_Temperature();
    double p = plantData->get_Pressure();
    double l = plantData->get_Lux();
    int soilPercent = std::clamp((626 - plantData->get_SoilMoisture()) * 100 / (626 - 347), 0, 100);

    // Definicja stylów CSS dla ułatwienia
    QString styleNormal = "color: #39FF14; font-weight: bold; border: none; background: transparent;"; // Zielony
    QString styleHot    = "color: #FF3131; font-weight: bold; border: none; background: transparent;"; // Czerwony (za wysoko)
    QString styleCold   = "color: #00FFFF; font-weight: bold; border: none; background: transparent;"; // Jasnoniebieski (za nisko)
    QString styleDark   = "color: #FFA500; font-weight: bold; border: none; background: transparent;"; // Pomarańczowy (za ciemno)

    // 1. TEMPERATURA (Przykładowo: 18°C - 28°C)
    if (QLabel *lTemp = m_dashboardPage->findChild<QLabel*>("tempValue")) {
        lTemp->setText(QString::number(t, 'f', 1) + " °C");
        if (t > 28.0) lTemp->setStyleSheet(styleHot);
        else if (t < 18.0) lTemp->setStyleSheet(styleCold);
        else lTemp->setStyleSheet(styleNormal);
    }

    // 2. WILGOTNOŚĆ (Przykładowo: 30% - 80%)
    if (QLabel *lSoil = m_dashboardPage->findChild<QLabel*>("soilValue")) {
        lSoil->setText(QString::number(soilPercent) + " %");
        if (soilPercent > 80) lSoil->setStyleSheet(styleHot); // Za mokro
        else if (soilPercent < 30) lSoil->setStyleSheet(styleCold); // Za sucho
        else lSoil->setStyleSheet(styleNormal);
    }

    // 3. ŚWIATŁO (Przykładowo: 100 lx - 1000 lx)
    if (QLabel *lLux = m_dashboardPage->findChild<QLabel*>("luxValue")) {
        lLux->setText(QString::number(l, 'f', 0) + " lx");
        if (l > 1000.0) lLux->setStyleSheet(styleHot); // Za ostre słońce
        else if (l < 100.0) lLux->setStyleSheet(styleDark); // Za ciemno
        else lLux->setStyleSheet(styleNormal);
    }

    // 4. CIŚNIENIE (Zostawiamy normalne, chyba że też chcesz limity)
    if (QLabel *lPres = m_dashboardPage->findChild<QLabel*>("presValue")) {
        lPres->setText(QString::number(p, 'f', 0) + " hPa");
        lPres->setStyleSheet(styleNormal);
    }

    updateGraphics();

    // Logika wykresów (co 10 sekund)
    static qint64 lastUpdateTime = 0;
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (currentTime - lastUpdateTime >= 10000) { 
        lastUpdateTime = currentTime;
        double vals[] = {t, (double)soilPercent, l, p};
        for(int i = 0; i < 4; ++i) {
            if (m_series[i]) m_series[i]->append(currentTime, vals[i]);
        }
        if (m_series[0]->count() > 1) {
            qint64 firstTs = m_series[0]->points().first().x() / 1000;
            qint64 lastTs = m_series[0]->points().last().x() / 1000;
            m_chartScroll->setMinimum(firstTs);
            m_chartScroll->setMaximum(lastTs);
            if (m_autoScroll) m_chartScroll->setValue(lastTs);
        }
        updateChartsWindow();
    }
}

void MainWindow::updateChartsWindow() {
    const int windowSizeSec = 900; // 15 min
    qint64 endSec = m_chartScroll->value();
    QDateTime endTime = QDateTime::fromMSecsSinceEpoch(endSec * 1000);
    QDateTime startTime = endTime.addSecs(-windowSizeSec);

    for(int i = 0; i < 4; ++i) {
        if (m_axesX[i]) m_axesX[i]->setRange(startTime, endTime);
        
        // Skalowanie Y do widocznych punktów
        double minV = 99999, maxV = -99999;
        bool found = false;
        for(auto pt : m_series[i]->points()) {
            if(pt.x() >= startTime.toMSecsSinceEpoch() && pt.x() <= endTime.toMSecsSinceEpoch()) {
                minV = std::min(minV, pt.y());
                maxV = std::max(maxV, pt.y());
                found = true;
            }
        }
        if(found && m_axesY[i]) {
            double margin = (maxV - minV) * 0.15 + 0.5;
            m_axesY[i]->setRange(minV - margin, maxV + margin);
        }
    }
}

void MainWindow::updateGraphics() {
    if (!plantData || !m_sunIcon || !m_plantAvatar) return;

    double t = plantData->get_Temperature();
    double l = plantData->get_Lux();
    int soilPercent = std::clamp((626 - plantData->get_SoilMoisture()) * 100 / (626 - 347), 0, 100);

    // Awatar
    QPixmap *avatar = &plantPixmap;
    if (soilPercent < 20) avatar = &plantDryPixmap;
    else if (soilPercent > 85) avatar = &plantWetPixmap;
    else if (t > 30.0) avatar = &plantHotPixmap;
    else if (t < 15.0) avatar = &plantColdPixmap;
    else if (l < 10.0) avatar = &plantDarkPixmap;

    int avatarSize = std::max(250, width() / 3);
    m_plantAvatar->setPixmap(avatar->scaled(avatarSize, avatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Ikony (Termometr, Kropla, Słońce)
    int baseSize = std::max(64, width() / 15);
    
    // Termometr z kolorem dynamicznym
    if (!thermoPixmap.isNull()) {
        QPixmap tPix = thermoPixmap.scaled(baseSize, baseSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap tCol(tPix.size()); tCol.fill(Qt::transparent);
        QPainter pT(&tCol);
        double tRatio = std::clamp((t - 15.0) / (35.0 - 15.0), 0.0, 1.0);
        QColor dynColor = QColor::fromHslF((1.0 - tRatio) * 0.6, 1.0, 0.5);
        pT.drawPixmap(0, 0, tPix);
        pT.setCompositionMode(QPainter::CompositionMode_SourceIn);
        pT.fillRect(tCol.rect(), dynColor);
        pT.end();
        m_thermometerIcon->setPixmap(tCol);
    }

    if (!pressurePixmap.isNull()) m_pressureIcon->setPixmap(pressurePixmap.scaled(baseSize, baseSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    if (!waterPixmap.isNull() && m_waterIcon) {
    // Kropelka rośnie wraz z wilgotnością (od 0.7x do 1.5x bazowego rozmiaru)
    double scaleFactor = 0.7 + (soilPercent / 100.0) * 0.8; 
    int dynamicSize = static_cast<int>(baseSize * scaleFactor);
    
    m_waterIcon->setPixmap(waterPixmap.scaled(dynamicSize, dynamicSize, 
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
    if (!sunPixmap.isNull()) {
        int alpha = std::clamp(static_cast<int>(50 + (l * 205 / 1000.0)), 50, 255);
        QPixmap sPix = sunPixmap.scaled(baseSize, baseSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap sTrans(sPix.size()); sTrans.fill(Qt::transparent);
        QPainter pS(&sTrans); pS.setOpacity(alpha / 255.0); pS.drawPixmap(0, 0, sPix);
        m_sunIcon->setPixmap(sTrans);
    }
}

void MainWindow::showPointValue(const QPointF &point, bool state) {
    if (state && m_tooltipLabel) {
        QString timeStr = QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss");
        m_tooltipLabel->setText(QString("Czas: %1\nWartość: %2").arg(timeStr).arg(point.y(), 0, 'f', 1));
        m_tooltipLabel->move(QCursor::pos() + QPoint(15, 15));
        m_tooltipLabel->show();
    } else if (m_tooltipLabel) {
        m_tooltipLabel->hide();
    }
}

void MainWindow::loadHistoryFromCSV() {
    QFile file("plant_history.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    in.readLine();

    while (!in.atEnd()) {
        QStringList f = in.readLine().split(';');
        if (f.size() < 5) continue;

        QDateTime dt = QDateTime::fromString(f[0], "yyyy-MM-dd hh:mm:ss");
        if (!dt.isValid()) continue;

        qint64 ts = dt.toMSecsSinceEpoch();

        if (m_series[0]) m_series[0]->append(ts, f[1].toDouble()); // temp
        if (m_series[1]) m_series[1]->append(ts, f[4].toDouble()); // soil
        if (m_series[2]) m_series[2]->append(ts, f[3].toDouble()); // lux
        if (m_series[3]) m_series[3]->append(ts, f[2].toDouble()); // pressure
    }

    file.close();
}

void MainWindow::clearHistory() {
    QFile file("plant_history.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "Data_Czas;Temperatura;Cisnienie;Swiatlo;Gleba_Procent\n";
        file.close();
    }
    for(int i=0; i<4; ++i) if(m_series[i]) m_series[i]->clear();
}

void MainWindow::saveToArchive() {
    QFile file("plant_history.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        int s = std::clamp((626 - plantData->get_SoilMoisture()) * 100 / (626 - 347), 0, 100);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ";" 
            << plantData->get_Temperature() << ";" << plantData->get_Pressure() << ";" 
            << plantData->get_Lux() << ";" << s << "\n";
        file.close();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // Obliczanie rozmiarów czcionek na podstawie szerokości okna
    int baseWidth = width();
    int valueFontSize = std::clamp(baseWidth / 35, 18, 45); // Duże cyfry
    int titleFontSize = std::clamp(baseWidth / 80, 10, 18); // Napisy nad cyframi

    QFont vFont; vFont.setPixelSize(valueFontSize); vFont.setBold(true);
    QFont tFont; tFont.setPixelSize(titleFontSize); tFont.setBold(true);

    // Aplikowanie do wartości (cyfr)
    QStringList values = {"tempValue", "presValue", "luxValue", "soilValue"};
    for (const QString &name : values) {
        if (QLabel *l = m_dashboardPage->findChild<QLabel*>(name)) {
            l->setFont(vFont);
        }
    }

    // Aplikowanie do tytułów kart
    QList<QLabel*> titles = m_dashboardPage->findChildren<QLabel*>("cardTitle");
    for (QLabel* l : titles) {
        l->setFont(tFont);
    }
    
    updateGraphics(); // Odświeżenie rozmiarów ikon i awatara
}

void MainWindow::toggleLanguage() {
    static bool isEn = true;
    isEn = !isEn;

    // Pobieramy wszystkie tytuły kart na dashboardzie
    QList<QLabel*> titles = m_dashboardPage->findChildren<QLabel*>("cardTitle");

    if (isEn) {
        m_tabs->setTabText(0, "Main Screen");
        m_tabs->setTabText(1, "Statistics");
        if(m_clearBtn) m_clearBtn->setText("CLEAR HISTORY");
        
        // Tłumaczenie tytułów wykresów
        if(m_charts[0]) m_charts[0]->setTitle("Temperature [°C]");
        if(m_charts[1]) m_charts[1]->setTitle("Moisture [%]");
        if(m_charts[2]) m_charts[2]->setTitle("Light [lx]");
        if(m_charts[3]) m_charts[3]->setTitle("Pressure [hPa]");

        for(auto t : titles) {
            QString txt = t->text().toUpper(); // Poprawione na toUpper()
            if(txt.contains("TEMPERATUR")) t->setText("TEMPERATURE");
            else if(txt.contains("CIŚNIENIE") || txt.contains("PRESS")) t->setText("PRESSURE");
            else if(txt.contains("JASNOŚĆ") || txt.contains("BRIGHT")) t->setText("BRIGHTNESS");
            else if(txt.contains("WILGOTNOŚĆ") || txt.contains("MOIST")) t->setText("MOISTURE");
        }
    } else {
        m_tabs->setTabText(0, "Ekran Główny");
        m_tabs->setTabText(1, "Statystyki");
        if(m_clearBtn) m_clearBtn->setText("WYCZYŚĆ HISTORIĘ");

        if(m_charts[0]) m_charts[0]->setTitle("Temperatura [°C]");
        if(m_charts[1]) m_charts[1]->setTitle("Wilgotność [%]");
        if(m_charts[2]) m_charts[2]->setTitle("Światło [lx]");
        if(m_charts[3]) m_charts[3]->setTitle("Ciśnienie [hPa]");

        for(auto t : titles) {
            QString txt = t->text().toUpper(); // Poprawione na toUpper()
            if(txt.contains("TEMPERATUR")) t->setText("TEMPERATURA");
            else if(txt.contains("PRESSURE") || txt.contains("CIŚN")) t->setText("CIŚNIENIE");
            else if(txt.contains("BRIGHTNESS") || txt.contains("JASN")) t->setText("JASNOŚĆ");
            else if(txt.contains("MOISTURE") || txt.contains("WILG")) t->setText("WILGOTNOŚĆ");
        }
    }
}

