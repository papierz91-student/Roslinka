\mainpage System Monitorowania Roślin – Dokumentacja Projektowa

\section intro_sec 1. Opis Projektu
Niniejszy system stanowi kompleksowe rozwiązanie do monitorowania warunków środowiskowych roślin. 
Głównym celem aplikacji jest zbieranie danych z sensorów, ich wizualizacja oraz archiwizacja 
w celu optymalizacji procesów uprawy.

\section hardware_sec 2. Architektura Sprzętowa
System składa się ze stacji pomiarowej (nadajnika) oraz aplikacji PC (odbiornika). 
Jednostka nadawcza odczytuje parametry z sensorów i przesyła je cyklicznie.

\image html device.jpg "Zdjęcie urządzenia nadawczego (stacja pomiarowa)" width=500cm

\section software_sec 3. Architektura Oprogramowania i Odwołania
Aplikacja została zaprojektowana w sposób modularny. Kluczowe klasy systemu to:

- \ref SerialHandler : Odpowiada za zarządzanie portem szeregowym i parsowanie danych.
- \ref PlantData : Model danych przechowujący aktualne wyniki pomiarów.
- \ref MainWindow : Klasa zarządzająca interfejsem graficznym i wykresami.

\section screens_sec 4. Prezentacja Interfejsu Graficznego
Poniżej przedstawiono główne ekrany aplikacji zrealizowanej w środowisku Qt.

\subsection dashboard_view 4.1 Panel Główny (Dashboard)
Widok ten pozwala na błyskawiczną ocenę stanu rośliny dzięki dynamicznym ikonom i awatarowi.
\image html app_screen1.png "Główny ekran aplikacji - Widok na żywo" width=500cm

\subsection stats_view 4.2 Analiza Historyczna
Sekcja statystyk oferuje interaktywne wykresy, które pozwalają śledzić trendy długofalowe.
\image html app_screen2.png "Ekran statystyk - Wykresy historyczne" width=500cm

\section protocol_sec 5. Protokół Komunikacyjny
Dane są przesyłane w formacie tekstowym ASCII, co znacząco ułatwia diagnostykę błędów oraz proces debugowania. Protokół wykorzystuje strukturę ramki o zmiennej długości z wyraźnymi znacznikami granicznymi.

**Struktura i formatowanie:**
- **Znaczniki ramki:** Każdy pakiet danych jest otoczony znakami `<` (początek) oraz `>` (koniec).
- **Separatory pól:** Parametry wewnątrz ramki są oddzielone średnikami `;`.
- **Identyfikatory:** Dane przesyłane są w formacie `KLUCZ:WARTOŚĆ`.

Przykładowa ramka danych:
\code
<T:22.5;P:1010.2;L:500;S:45;CS:15>
\endcode

**Specyfikacja pól pomiarowych:**
- **T (Temperature):** Temperatura otoczenia z czujnika, podawana z precyzją do jednego miejsca po przecinku [°C].
- **P (Pressure):** Ciśnienie atmosferyczne [hPa].
- **L (Light):** Natężenie światła mierzone przez fotorezystor/czujnik natężenia [lux].
- **S (Soil/Status):** Poziom wilgotności gleby  [%].
- **CS (Checksum):** Suma kontrolna wyliczana jako prosta suma bajtów lub operacja XOR, służąca do odrzucania błędnych pakietów.

Każda odebrana ramka jest weryfikowana pod kątem poprawności przez metodę \ref SerialHandler::validateChecksum. W przypadku wykrycia błędnej sumy kontrolnej lub niekompletnej struktury, dane są ignorowane, aby zapobiec przekłamaniom na wykresach.

\note Należy pamiętać o regularnym czyszczeniu archiwum CSV w przypadku zbierania danych z dużą częstotliwością.

\author Michał Papierzański
\date 2026-05-05