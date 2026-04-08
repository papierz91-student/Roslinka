#include <QApplication>
#include "MainWindow.hpp"
#include "PlantData.hpp"

int main(int argc, char *argv[]) {
    PlantData myPlant;
    QApplication app(argc, argv);
    MainWindow window(&myPlant);
    window.show();
    return app.exec();
}