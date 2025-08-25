#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../inc/AppController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    AppController controller;
    engine.rootContext()->setContextProperty("appController", &controller);

    engine.load(QUrl::fromLocalFile("main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
