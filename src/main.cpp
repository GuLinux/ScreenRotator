#include <QApplication>
#include "displaymanager.h"
#include "orientationsensor.h"
#include "trayicon.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DisplayManager displayManager;
    TrayIcon tray;
    return app.exec();
}
