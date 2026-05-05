/**
 * @file PlantData.hpp
 * @brief Definicja klasy modelu danych pomiarowych.
 * @author Michał Papierzański
 * @date 2026-05-05
 */

#ifndef PLANTDATA_H
#define PLANTDATA_H

/**
 * @class PlantData
 * @brief Model danych przechowujący parametry życiowe rośliny pobrane z sensorów.
 * 
 * Klasa stanowi centralny punkt składowania danych w aplikacji. Jest obiektem 
 * współdzielonym ("data container"), do którego SerialHandler zapisuje nowe wartości, 
 * a MainWindow odczytuje je w celu aktualizacji interfejsu graficznego.
 * 
 * @note Klasa nie zawiera logiki biznesowej, służy jedynie jako bezpieczny kontener 
 * dla surowych i przeliczonych wartości pomiarowych.
 */
class PlantData {
public:
    /**
     * @brief Konstruktor domyślny.
     * 
     * Inicjalizuje wszystkie parametry numeryczne wartościami bezpiecznymi (0.0 lub 0),
     * co zapobiega wyświetlaniu nieokreślonych wartości przed odebraniem pierwszej ramki danych.
     */
    PlantData() : temperature(0.0), pressure(0.0), lux(0.0), soilMoisture(0) {}

    /** @name Sety (Metody modyfikujące)
     * Metody wykorzystywane głównie przez SerialHandler do aktualizacji modelu.
     */
    ///@{
    
    /**
     * @brief Aktualizuje wartość temperatury.
     * @param temp Temperatura otoczenia w stopniach Celsjusza [°C].
     */
    void set_Temperature(double temp) { temperature = temp; }

    /**
     * @brief Aktualizuje wartość ciśnienia.
     * @param pres Ciśnienie atmosferyczne [hPa].
     */
    void set_Pressure(double pres) { pressure = pres; }

    /**
     * @brief Aktualizuje natężenie światła.
     * @param light Wartość natężenia w luksach [lx].
     */
    void set_Lux(double light) { lux = light; }

    /**
     * @brief Aktualizuje poziom wilgotności podłoża.
     * @param moisture Procentowa wilgotność gleby [0-100].
     */
    void set_SoilMoisture(int moisture) { soilMoisture = moisture; }
    ///@}

    /** @name Gety (Metody dostępowe)
     * Metody wykorzystywane przez interfejs użytkownika oraz moduł archiwizacji danych.
     */
    ///@{

    /**
     * @brief Zwraca ostatnio zarejestrowaną temperaturę.
     * @return double Wartość w [°C].
     */
    double get_Temperature() const { return temperature; }

    /**
     * @brief Zwraca ostatnio zarejestrowane ciśnienie.
     * @return double Wartość w [hPa].
     */
    double get_Pressure() const { return pressure; }

    /**
     * @brief Zwraca ostatnio zarejestrowane natężenie światła.
     * @return double Wartość w [lx].
     */
    double get_Lux() const { return lux; }

    /**
     * @brief Zwraca ostatnio zarejestrowaną wilgotność gleby.
     * @return int Procentowa zawartość wody w podłożu.
     */
    int get_SoilMoisture() const { return soilMoisture; } 
    ///@}

private:
    double temperature;   /**< Temperatura bieżąca [°C]. */
    double pressure;      /**< Ciśnienie atmosferyczne [hPa]. */
    double lux;           /**< Natężenie światła padającego [lx]. */
    int soilMoisture;     /**< Wilgotność gleby (wartość zmapowana na procenty). */
};

#endif // PLANTDATA_H