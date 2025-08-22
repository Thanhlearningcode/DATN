#include "MainWindow.h"
#include "Simulation.h"
#include "qcustomplot.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QStatusBar>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setWindowTitle("Vibration Analyser");

    // Left panel
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QGroupBox *comGroup = new QGroupBox("COM:");
    QVBoxLayout *comLayout = new QVBoxLayout;
    QComboBox *comCombo = new QComboBox;
    comCombo->addItems({"ReCOM", "COM1", "COM2", "COM3"});
    comLayout->addWidget(comCombo);
    comGroup->setLayout(comLayout);
    leftLayout->addWidget(comGroup);

    QPushButton *realTimeBtn = new QPushButton("RealTime");
    leftLayout->addWidget(realTimeBtn);

    QGroupBox *logGroup = new QGroupBox("Analys Log");
    QVBoxLayout *logLayout = new QVBoxLayout;
    QPushButton *fileBtn = new QPushButton("...");
    logLayout->addWidget(fileBtn);
    QLabel *fileLabel = new QLabel("File Name");
    logLayout->addWidget(fileLabel);
    QLabel *folderLabel = new QLabel("Folder name");
    logLayout->addWidget(folderLabel);
    QSpinBox *timeLogSpin = new QSpinBox;
    timeLogSpin->setValue(12);
    logLayout->addWidget(new QLabel("Time Log (s):"));
    logLayout->addWidget(timeLogSpin);
    QSpinBox *timeSaveSpin = new QSpinBox;
    timeSaveSpin->setValue(10);
    logLayout->addWidget(new QLabel("Thời gian lưu (p):"));
    logLayout->addWidget(timeSaveSpin);
    QPushButton *logSingleBtn = new QPushButton("Log Single");
    QPushButton *logAutoBtn = new QPushButton("Log Auto");
    QPushButton *stopBtn = new QPushButton("Stop");
    logLayout->addWidget(logSingleBtn);
    logLayout->addWidget(logAutoBtn);
    logLayout->addWidget(stopBtn);
    QLabel *statusLabel = new QLabel("Status Log: <font color='red'>Disable</font>");
    logLayout->addWidget(statusLabel);
    logGroup->setLayout(logLayout);
    leftLayout->addWidget(logGroup);

    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->addLayout(leftLayout);

    // Right panel
    QVBoxLayout *rightLayout = new QVBoxLayout;
    QGridLayout *topGrid = new QGridLayout;
    topGrid->addWidget(new QLabel("Plot Top"), 0, 0);
    QComboBox *plotTopCombo = new QComboBox;
    plotTopCombo->addItems({"Ch1", "Ch2"});
    topGrid->addWidget(plotTopCombo, 0, 1);
    topGrid->addWidget(new QLabel("Sensivity"), 0, 2);
    QComboBox *sensTopCombo = new QComboBox;
    sensTopCombo->addItems({"None", "Low", "Medium", "High"});
    topGrid->addWidget(sensTopCombo, 0, 3);
    topGrid->addWidget(new QLabel("Time Samp"), 0, 4);
    QSpinBox *timeSampTop = new QSpinBox;
    timeSampTop->setRange(100, 10000);
    timeSampTop->setValue(1000);
    topGrid->addWidget(timeSampTop, 0, 5);
    topGrid->addWidget(new QLabel("Plot Bottom"), 1, 0);
    QComboBox *plotBotCombo = new QComboBox;
    plotBotCombo->addItems({"Ch1", "Ch2"});
    topGrid->addWidget(plotBotCombo, 1, 1);
    topGrid->addWidget(new QLabel("Sensivity"), 1, 2);
    QComboBox *sensBotCombo = new QComboBox;
    sensBotCombo->addItems({"None", "Low", "Medium", "High"});
    topGrid->addWidget(sensBotCombo, 1, 3);
    topGrid->addWidget(new QLabel("Time Samp"), 1, 4);
    QSpinBox *timeSampBot = new QSpinBox;
    timeSampBot->setRange(100, 10000);
    timeSampBot->setValue(1000);
    topGrid->addWidget(timeSampBot, 1, 5);
    rightLayout->addLayout(topGrid);

    // FFT and channel
    QHBoxLayout *fftLayout = new QHBoxLayout;
    fftLayout->addWidget(new QLabel("Channel"));
    QCheckBox *chT = new QCheckBox("T");
    chT->setChecked(true);
    fftLayout->addWidget(chT);
    QCheckBox *chB = new QCheckBox("B");
    fftLayout->addWidget(chB);
    fftLayout->addWidget(new QLabel("FFT samp (KSPS):"));
    QSpinBox *fftSpin = new QSpinBox;
    fftSpin->setValue(50);
    fftLayout->addWidget(fftSpin);
    rightLayout->addLayout(fftLayout);

    // Tạo 4 plot mô phỏng realtime
    setupSimulationPlot(simPlot, tData, yData, simIndex, Qt::blue);
    setupSimulationPlot(simPlot2, tData2, yData2, simIndex2, Qt::red);
    setupSimulationPlot(simPlot3, tData3, yData3, simIndex3, Qt::green);
    setupSimulationPlot(simPlot4, tData4, yData4, simIndex4, Qt::magenta);
    rightLayout->addWidget(simPlot);
    rightLayout->addWidget(simPlot2);
    rightLayout->addWidget(simPlot3);
    rightLayout->addWidget(simPlot4);

    mainLayout->addLayout(rightLayout);

    // Khởi tạo timer cho từng plot
    simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    simTimer->start(20);

    simTimer2 = new QTimer(this);
    connect(simTimer2, &QTimer::timeout, this, &MainWindow::updateSimulation2);
    simTimer2->start(20);

    simTimer3 = new QTimer(this);
    connect(simTimer3, &QTimer::timeout, this, &MainWindow::updateSimulation3);
    simTimer3->start(20);

    simTimer4 = new QTimer(this);
    connect(simTimer4, &QTimer::timeout, this, &MainWindow::updateSimulation4);
    simTimer4->start(20);
}

void MainWindow::setupSimulationPlot(QCustomPlot*& plot, QVector<double>& t, QVector<double>& y, int& idx, QColor color) {
    plot = new QCustomPlot;
    plot->addGraph();
    plot->xAxis->setLabel("Time [s]");
    plot->yAxis->setLabel("Amplitude");
    plot->graph(0)->setPen(QPen(color));
    t.clear();
    y.clear();
    idx = 0;
}

void MainWindow::updateSimulation() {
    int batch = 100;
    for (int i = 0; i < batch; ++i) {
        double t = simIndex / fs;
        double val = std::sin(2 * M_PI * f1 * t) + std::sin(2 * M_PI * f2 * t);
        tData.append(t);
        yData.append(val);
        simIndex++;
        if (t > duration) {
            simTimer->stop();
            break;
        }
    }
    simPlot->graph(0)->setData(tData, yData);
    simPlot->xAxis->setRange(0, duration);
    simPlot->yAxis->rescale();
    simPlot->replot();
}

void MainWindow::updateSimulation2() {
    int batch = 100;
    for (int i = 0; i < batch; ++i) {
        double t = simIndex2 / fs;
        double val = std::sin(2 * M_PI * (f1+100) * t) + std::sin(2 * M_PI * (f2+100) * t);
        tData2.append(t);
        yData2.append(val);
        simIndex2++;
        if (t > duration) {
            simTimer2->stop();
            break;
        }
    }
    simPlot2->graph(0)->setData(tData2, yData2);
    simPlot2->xAxis->setRange(0, duration);
    simPlot2->yAxis->rescale();
    simPlot2->replot();
}

void MainWindow::updateSimulation3() {
    int batch = 100;
    for (int i = 0; i < batch; ++i) {
        double t = simIndex3 / fs;
        double val = std::sin(2 * M_PI * (f1+200) * t) + std::sin(2 * M_PI * (f2+200) * t);
        tData3.append(t);
        yData3.append(val);
        simIndex3++;
        if (t > duration) {
            simTimer3->stop();
            break;
        }
    }
    simPlot3->graph(0)->setData(tData3, yData3);
    simPlot3->xAxis->setRange(0, duration);
    simPlot3->yAxis->rescale();
    simPlot3->replot();
}

void MainWindow::updateSimulation4() {
    int batch = 100;
    for (int i = 0; i < batch; ++i) {
        double t = simIndex4 / fs;
        double val = std::sin(2 * M_PI * (f1+300) * t) + std::sin(2 * M_PI * (f2+300) * t);
        tData4.append(t);
        yData4.append(val);
        simIndex4++;
        if (t > duration) {
            simTimer4->stop();
            break;
        }
    }
    simPlot4->graph(0)->setData(tData4, yData4);
    simPlot4->xAxis->setRange(0, duration);
    simPlot4->yAxis->rescale();
    simPlot4->replot();
}

MainWindow::~MainWindow() {
    // Giải phóng các đối tượng động để tránh memory leak
    delete simPlot;
    delete simPlot2;
    delete simPlot3;
    delete simPlot4;
    delete simTimer;
    delete simTimer2;
    delete simTimer3;
    delete simTimer4;
}
