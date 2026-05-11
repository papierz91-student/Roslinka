/**
 * \file MainWindow.hpp
 * \brief Główny interfejs graficzny aplikacji monitorującej stan rośliny.
 * \author Michał Papierzański
 * \date 2026-05-05
 */

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

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

#include "SerialHandler.hpp"
#include "PlantData.hpp"

QT_USE_NAMESPACE

/**
 * \class MainWindow
 * \brief Główna klasa okna aplikacji odpowiedzialna za wizualizację danych z sensorów.
 * 
 * Klasa integruje dane z obiektu \link PlantData PlantData \endlink, zarządza połączeniem 
 * szeregowym (\link SerialHandler SerialHandler \endlink) oraz prezentuje wyniki w formie 
 * graficznych kart, interaktywnych wykresów i dynamicznego awatara stanu rośliny. 
 * Obsługuje również archiwizację danych do plików CSV.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * \brief Konstruktor głównego okna.
     * \param[in] data - Wskaźnik do obiektu przechowującego aktualne dane pomiarowe.
     * \param[in] parent - Wskaźnik do obiektu rodzica (Qt).
     */
    MainWindow(PlantData *data, QWidget *parent = nullptr);

    /**
     * \brief Domyślny destruktor klasy MainWindow.
     */
    ~MainWindow() = default;

public slots:
    /**
     * \brief Aktualizuje zakres i skalowanie osi na wykresach historycznych.
     * 
     * Dostosowuje widok do okna czasowego zdefiniowanego przez pasek przewijania.
     */
    void updateChartsWindow();

    /**
     * \brief Pobiera najnowsze dane z obiektu PlantData i odświeża interfejs.
     * 
     * Aktualizuje etykiety tekstowe, wywołuje przeliczenie grafiki oraz dodaje punkty do wykresów.
     */
    void updateDisplay();

    /**
     * \brief Zapisuje aktualne parametry do pliku plant_history.csv.
     * 
     * Archiwizuje temperaturę, ciśnienie, światło i wilgotność. Wywoływana 
     * w momencie dodawania danych do wykresu.
     */
    void saveToArchive();

    /**
     * \brief Czyści historię pomiarów w interfejsie oraz nadpisuje plik CSV pustym nagłówkiem.
     */
    void clearHistory();

    /**
     * \brief Obsługuje wyświetlanie dymka (tooltip) z wartością po najechaniu na punkt na wykresie.
     * \param[in] point - Współrzędne punktu na wykresie.
     * \param[in] state - Stan najechania.
     * \retval true - kursor znajduje się nad punktem.
     * \retval false - kursor znajduje się poza punktem.
     */
    void showPointValue(const QPointF &point, bool state);

    /**
     * \brief Wczytuje dane historyczne z pliku CSV przy starcie aplikacji i wypełnia nimi wykresy.
     */
    void loadHistoryFromCSV();

    /**
     * \brief Przełącza język interfejsu pomiędzy polskim a angielskim.
     * 
     * Zmienia etykiety kart, nagłówki zakładek oraz opisy osi wykresów.
     */
    void toggleLanguage();

protected:
    /**
     * \brief Przeciążona metoda obsługująca zmianę rozmiaru okna.
     * \param[in] event - Obiekt zdarzenia zmiany rozmiaru zawierający nowe wymiary okna.
     * 
     * Dynamicznie przelicza wielkość czcionek i ikon, aby zachować responsywność interfejsu.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * \brief Aktualizuje grafikę awatara rośliny oraz kolorystykę ikon na podstawie progów alarmowych.
     * 
     * Zmienia stan awatara (np. na 'hot', 'dry', 'dark') w zależności od warunków środowiskowych.
     */
    void updateGraphics();

private:
    /** \brief Inicjalizuje i układa elementy na głównej zakładce (Dashboard). */
    void setupDashboard();

    /** \brief Inicjalizuje sekcję statystyk, tworzy wykresy QtCharts i konfiguruje ich osie. */
    void setupStats();

    /**
     * \brief Tworzy ustandaryzowany widżet karty dla pojedynczego parametru.
     * \param[in] title - Tytuł wyświetlany na górze karty.
     * \param[in,out] icon - Wskaźnik do etykiety z ikoną.
     * \param[in,out] value - Wskaźnik do etykiety, w której będzie wyświetlana wartość.
     * \return Wskaźnik do gotowego, ostylowanego obiektu QWidget reprezentującego kartę.
     */
    QWidget* createCard(const QString &title, QLabel *icon, QLabel *value);

    // Zarządzanie danymi i komunikacją
    SerialHandler *serialManager; ///< Menadżer komunikacji przez port szeregowy.
    PlantData *plantData;         ///< Model danych z sensorów.
    QTimer *m_portCheckTimer;     ///< Timer monitorujący stan połączenia.

    int m_measurementCounter = 0; ///< Licznik wykonanych pomiarów.

    // Elementy struktury UI
    QTabWidget *m_tabs;           ///< Główny widżet zakładek.
    QWidget *m_dashboardPage;     ///< Strona główna z podglądem na żywo.
    QWidget *m_statsPage;         ///< Strona z wykresami historycznymi.

    // Ikony i wizualizacja
    QLabel *m_sunIcon;            ///< Ikona natężenia światła.
    QLabel *m_waterIcon;          ///< Ikona wilgotności (skalowana dynamicznie).
    QLabel *m_thermometerIcon;    ///< Ikona temperatury (zmieniająca kolor).
    QLabel *m_pressureIcon;       ///< Ikona ciśnienia.
    QLabel *m_plantAvatar;        ///< Główny obrazek stanu rośliny.

    QPushButton *m_langBtn;       ///< Przycisk zmiany języka.

    // Zasoby graficzne
    QPixmap sunPixmap, waterPixmap, thermoPixmap, pressurePixmap;
    QPixmap plantPixmap, plantHotPixmap, plantColdPixmap;
    QPixmap plantDryPixmap, plantWetPixmap, plantDarkPixmap;

    // Komponenty wykresów
    QChartView *m_chartViews[4];  ///< Tablica widoków dla 4 głównych wykresów.
    QChart *m_charts[4];          ///< Obiekty wykresów (Temp, Wilg, Światło, Ciśn).
    QLineSeries *m_series[4];     ///< Serie danych czasowych.
    QDateTimeAxis *m_axesX[4];    ///< Osie czasu (X).
    QValueAxis *m_axesY[4];       ///< Osie wartości (Y).

    QPushButton *m_clearBtn;      ///< Przycisk czyszczenia historii.
    QLabel *m_tooltipLabel;       ///< Etykieta wyświetlająca szczegóły punktu na wykresie.

    QScrollBar *m_chartScroll;    ///< Pasek przewijania osi czasu na wykresach.
    
    /** \brief Flaga określająca, czy wykres ma automatycznie śledzić najnowsze dane. */
    bool m_autoScroll = true;     
};

#endif // MAINWINDOW_H