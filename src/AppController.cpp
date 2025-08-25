#include "AppController.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

AppController::AppController(QObject *parent) : QObject(parent) {}

void AppController::logSensorData() {
    QDir dir;
    if (!dir.exists("log")) {
        dir.mkdir("log");
    }
    QFile file("log/sensor_log.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Time,Value\n";
        // TODO: Lấy dữ liệu thật từ DAQWorker hoặc buffer
        // out << ...
        file.close();
        emit logStatusChanged("Status Log: <font color='green'>Log thành công</font>");
    } else {
        emit logStatusChanged("Status Log: <font color='red'>Log lỗi</font>");
    }
}


#include "DAQWorker.h"
void AppController::refreshDevice() {
    QStringList devs = DAQWorker::availableDevices();
    if (devs.isEmpty()) {
        emit deviceStatusChanged("<font color='red'>Device: Không tìm thấy thiết bị DAQ hoặc lỗi driver!</font>");
    } else {
        QString d = devs.first();
        emit deviceStatusChanged(QString("<font color='green'>Device: %1 — Connected</font>").arg(d));
    }
}
