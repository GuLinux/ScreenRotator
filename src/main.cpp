#include <QApplication>
#include "displaymanager.h"
#include "orientationsensor.h"
#include "trayicon.h"
#include "rotateinput.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DisplayManager displayManager;
    TrayIcon tray;
    OrientationSensor sensor;
    RotateInput rotateInput;
    QObject::connect(&sensor, &OrientationSensor::reading, &displayManager, &DisplayManager::setOrientation);
    QObject::connect(&sensor, &OrientationSensor::reading, &rotateInput, &RotateInput::rotate);
    return app.exec();
}
