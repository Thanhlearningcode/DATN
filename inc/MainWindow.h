#pragma once
#include <QMainWindow>

class QCustomPlot;


#include <QTimer>
#include <vector>
#include <QVector>
class QLabel;
class DAQWorker;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    QLabel *statusLabel = nullptr;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTimer *simTimer = nullptr;
    QTimer *simTimer2 = nullptr;
    QTimer *simTimer3 = nullptr;
    QTimer *simTimer4 = nullptr;
    QCustomPlot *simPlot = nullptr;
    QCustomPlot *simPlot2 = nullptr;
    QCustomPlot *simPlot3 = nullptr;
    QCustomPlot *simPlot4 = nullptr;
    QVector<double> tData, yData;
    QVector<double> tData2, yData2;
    QVector<double> tData3, yData3;
    QVector<double> tData4, yData4;
    double fs = 20000;
    double duration = 1.0;
    double f1 = 1000;
    double f2 = 1100;
    int simIndex = 0;
    int simIndex2 = 0;
    int simIndex3 = 0;
    int simIndex4 = 0;
    void setupSimulationPlot(QCustomPlot*& plot, QVector<double>& t, QVector<double>& y, int& idx, QColor color);
    void updateDeviceStatus();
    // DAQ integration
    DAQWorker *daqWorker = nullptr;
    QLabel *deviceStatusLabel = nullptr;

private slots:
    void onDaqSamples(const QVector<double> &times, const QVector<double> &values);
    void updateSimulation2();
    void updateSimulation3();
    void updateSimulation4();
    void logSensorData();
};
