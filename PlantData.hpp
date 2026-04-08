#ifndef PLANTDATA_H
#define PLANTDATA_H

/**
 * @class PlantData
 * @brief Klasa reprezentująca zestaw danych pomiarowych z czujników monitorujących roślinę.
 * * Klasa przechowuje informacje o warunkach atmosferycznych oraz stanie podłoża,
 * udostępniając interfejs do zapisu i odczytu tych parametrów.
 */
class PlantData {
public:
    /** @name Zasoby systemowe (Pola danych) */
    ///@{
    double temperature;   ///< Aktualna temperatura [°C].
    double pressure;      ///< Aktualne ciśnienie atmosferyczne [hPa].
    double lux;           ///< Natężenie światła [lx].
    int soilMoisture;     ///< Wilgotność gleby [%].        
    ///@}

    /**
     * @brief Konstruktor domyślny.
     * Inicjalizuje wszystkie parametry wartościami zerowymi.
     */
    PlantData() : temperature(0.0), pressure(0.0), lux(0.0), soilMoisture(0) {}

    /**
     * @brief Ustawia nową wartość temperatury.
     * @param temp Temperatura w stopniach Celsjusza.
     */
    void set_Temperature(double temp) { temperature = temp; }

    /**
     * @brief Ustawia nową wartość ciśnienia.
     * @param pres Ciśnienie atmosferyczne w hektopaskalach [hPa].
     */
    void set_Pressure(double pres) { pressure = pres; }

    /**
     * @brief Ustawia nową wartość natężenia światła.
     * @param light Natężenie światła w luksach [lx].
     */
    void set_Lux(double light) { lux = light; }

    /**
     * @brief Ustawia nową wartość wilgotności gleby.
     * @param moisture Wilgotność gleby wyrażona w procentach [0-100].
     */
    void set_SoilMoisture(int moisture) { soilMoisture = moisture; }

    /**
     * @brief Pobiera aktualną temperaturę.
     * @return Wartość temperatury w [°C].
     */
    double get_Temperature() const { return temperature; }

    /**
     * @brief Pobiera aktualne ciśnienie.
     * @return Wartość ciśnienia w [hPa].
     */
    double get_Pressure() const { return pressure; }

    /**
     * @brief Pobiera aktualne natężenie światła.
     * @return Wartość natężenia światła w [lx].
     */
    double get_Lux() const { return lux; }

    /**
     * @brief Pobiera aktualną wilgotność gleby.
     * @return Procentowa wilgotność gleby [%].
     */
    int get_SoilMoisture() const { return soilMoisture; } 
};

#endif // PLANTDATA_H