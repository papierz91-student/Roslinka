#include <QApplication>
#include "MainWindow.h"
#include "PlantData.h"

int main(int argc, char *argv[]) {
    PlantData myPlant;
    QApplication app(argc, argv);
    MainWindow window(&myPlant);
    window.show();
    return app.exec();
}