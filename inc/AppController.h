#pragma once
#include <QObject>
#include <QString>

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);

    Q_INVOKABLE void logSensorData();
    Q_INVOKABLE void refreshDevice();

signals:
    void deviceStatusChanged(const QString &status);
    void logStatusChanged(const QString &status);
};
