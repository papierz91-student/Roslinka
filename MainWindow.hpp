#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLCDNumber>
#include <QTabWidget>
#include <QResizeEvent>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>

// Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

#include "SerialHandler.hpp"
#include "PlantData.hpp"

QT_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(PlantData *data, QWidget *parent = nullptr);
    ~MainWindow() = default;

public slots:
    void updateChartsWindow();
    void updateDisplay();
    void saveToArchive();
    void clearHistory();
    void showPointValue(const QPointF &point, bool state);
    void loadHistoryFromCSV();
    void toggleLanguage();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void updateGraphics();

private:
    void setupDashboard();
    void setupStats();
    QWidget* createCard(const QString &title, QLabel *icon, QLabel *value);

    SerialHandler *serialManager;
    PlantData *plantData;
    QTimer *archiveTimer;
    QTimer *m_portCheckTimer;

    int m_measurementCounter = 0;

    QTabWidget *m_tabs;
    QWidget *m_dashboardPage;
    QWidget *m_statsPage;

    QLabel *m_sunIcon;
    QLabel *m_waterIcon;
    QLabel *m_thermometerIcon;
    QLabel *m_pressureIcon;
    QLabel *m_plantAvatar;

    QPushButton *m_langBtn;

    QPixmap sunPixmap, waterPixmap, thermoPixmap, pressurePixmap;
    QPixmap plantPixmap, plantHotPixmap, plantColdPixmap;
    QPixmap plantDryPixmap, plantWetPixmap, plantDarkPixmap;

    QChartView *m_chartViews[4];
    QChart *m_charts[4];
    QLineSeries *m_series[4];
    QDateTimeAxis *m_axesX[4];
    QValueAxis *m_axesY[4];

    QPushButton *m_clearBtn;
    QLabel *m_tooltipLabel;

    QScrollBar *m_chartScroll;
    bool m_autoScroll = true;
};

#endif