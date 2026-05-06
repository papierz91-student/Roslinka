/**
 * \file PlantData.hpp
 * \brief Definicja klasy modelu danych pomiarowych.
 * \author Michał Papierzański
 * \date 2026-05-05
 */

#ifndef PLANTDATA_H
#define PLANTDATA_H

/**
 * \brief Model danych przechowujący parametry życiowe rośliny pobrane z sensorów.
 * 
 * Klasa stanowi centralny punkt składowania danych w aplikacji. Jest obiektem 
 * współdzielonym ("data container"), do którego \link SerialHandler SerialHandler \endlink 
 * zapisuje nowe wartości, a \link MainWindow MainWindow \endlink odczytuje je 
 * w celu aktualizacji interfejsu graficznego.
 * 
 * \note Klasa nie zawiera logiki biznesowej, służy jedynie jako bezpieczny kontener 
 * dla surowych i przeliczonych wartości pomiarowych.
 */
class PlantData {
public:
    /**
     * \brief Konstruktor domyślny.
     * 
     * Inicjalizuje wszystkie parametry numeryczne wartościami bezpiecznymi (0.0 lub 0),
     * co zapobiega wyświetlaniu nieokreślonych wartości przed odebraniem pierwszej ramki danych.
     */
    PlantData() : temperature(0.0), pressure(0.0), lux(0.0), soilMoisture(0) {}

    /** \name Sety (Metody modyfikujące)
     * Metody wykorzystywane głównie przez SerialHandler do aktualizacji modelu.
     */
    ///@{
    
    /**
     * \brief Aktualizuje wartość temperatury.
     * \param[in] temp - Temperatura otoczenia w stopniach Celsjusza [°C].
     */
    void set_Temperature(double temp) { temperature = temp; }

    /**
     * \brief Aktualizuje wartość ciśnienia.
     * \param[in] pres - Ciśnienie atmosferyczne [hPa].
     */
    void set_Pressure(double pres) { pressure = pres; }

    /**
     * \brief Aktualizuje natężenie światła.
     * \param[in] light - Wartość natężenia w luksach [lx].
     */
    void set_Lux(double light) { lux = light; }

    /**
     * \brief Aktualizuje poziom wilgotności podłoża.
     * \param[in] moisture - Procentowa wilgotność gleby [0-100].
     */
    void set_SoilMoisture(int moisture) { soilMoisture = moisture; }
    ///@}

    /** \name Gety (Metody dostępowe)
     * Metody wykorzystywane przez interfejs użytkownika oraz moduł archiwizacji danych.
     */
    ///@{

    /**
     * \brief Zwraca ostatnio zarejestrowaną temperaturę.
     * \return Wartość temperatury w [°C].
     */
    double get_Temperature() const { return temperature; }

    /**
     * \brief Zwraca ostatnio zarejestrowane ciśnienie.
     * \return Wartość ciśnienia w [hPa].
     */
    double get_Pressure() const { return pressure; }

    /**
     * \brief Zwraca ostatnio zarejestrowane natężenie światła.
     * \return Wartość natężenia w [lx].
     */
    double get_Lux() const { return lux; }

    /**
     * \brief Zwraca ostatnio zarejestrowaną wilgotność gleby.
     * \return Procentowa zawartość wody w podłożu [%].
     */
    int get_SoilMoisture() const { return soilMoisture; } 
    ///@}

private:
    /** \brief Temperatura bieżąca [°C]. */
    double temperature;   
    /** \brief Ciśnienie atmosferyczne [hPa]. */
    double pressure;      
    /** \brief Natężenie światła padającego [lx]. */
    double lux;           
    /** \brief Wilgotność gleby (wartość zmapowana na procenty [%]). */
    int soilMoisture;     
};

#endif // PLANTDATA_H