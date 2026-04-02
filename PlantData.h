#ifndef PLANTDATA_H
#define PLANTDATA_H

class PlantData {
public:
    // Zmienne przechowujące stan czujników (Zasoby systemowe)
    double temperature;   ///< Aktualna temperatura [°C].
    double pressure;      ///< Aktualne ciśnienie atmosferyczne [hPa].
    double lux;           ///< Natężenie światła [lx].
    int soilMoisture;     ///< Wilgotność gleby [%].        

    PlantData() : temperature(0.0), pressure(0.0), lux(0.0), soilMoisture(0) {}

    void set_Temperature(double temp) { temperature = temp; }
    void set_Pressure(double pres) { pressure = pres; }
    void set_Lux(double light) { lux = light; }
    void set_SoilMoisture(int moisture) { soilMoisture = moisture; }

    double get_Temperature() const { return temperature; }
    double get_Pressure() const { return pressure; }
    double get_Lux() const { return lux; }
    int get_SoilMoisture() const { return soilMoisture; } 
};

#endif // PLANTDATA_H