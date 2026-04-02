cmake_minimum_required(VERSION 3.16)
project(ArduinoMonitor VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ArduinoMonitor
    main.cpp
    MainWindow.cpp
    SerialHandler.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)
target_link_libraries(ArduinoMonitor PRIVATE 
    Qt6::Widgets 
    Qt6::SerialPort
)