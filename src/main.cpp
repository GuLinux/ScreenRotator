#include <QGuiApplication>
#include "displaymanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    DisplayManager displayManager;
    return app.exec();
}
