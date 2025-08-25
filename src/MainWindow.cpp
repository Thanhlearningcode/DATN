#include "MainWindow.h"
#include "Simulation.h"
#include "qcustomplot.h"
#include "DAQWorker.h"
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
#include <QFont>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setWindowTitle("Vibration Analyser");
    // Start the main window in full screen (or change to showMaximized())
    setWindowState(Qt::WindowFullScreen);

    // Left panel
    QVBoxLayout *leftLayout = new QVBoxLayout;
    // Device status group (replaces COM selection)
    QGroupBox *deviceGroup = new QGroupBox("Device");
    QVBoxLayout *deviceLayout = new QVBoxLayout;
    deviceStatusLabel = new QLabel("Checking...");
    deviceStatusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    deviceStatusLabel->setMinimumHeight(24);
    deviceLayout->addWidget(deviceStatusLabel);
    QPushButton *refreshDevicesBtn = new QPushButton("Refresh");
    deviceLayout->addWidget(refreshDevicesBtn);
    deviceGroup->setLayout(deviceLayout);
    leftLayout->addWidget(deviceGroup);

    // Timer to auto-refresh device list/status
    QTimer *deviceTimer = new QTimer(this);
    deviceTimer->setInterval(2000); // every 2s
    connect(deviceTimer, &QTimer::timeout, this, [this]() { updateDeviceStatus(); });
    deviceTimer->start();
    connect(refreshDevicesBtn, &QPushButton::clicked, this, [this]() { updateDeviceStatus(); });
    updateDeviceStatus();
    // RealTime button and DAQ worker
    QPushButton *realTimeBtn = new QPushButton("RealTime");
    leftLayout->addWidget(realTimeBtn);
    daqWorker = new DAQWorker(this);
    connect(daqWorker, &DAQWorker::samplesReady, this, &MainWindow::onDaqSamples);
    connect(realTimeBtn, &QPushButton::clicked, [this, realTimeBtn]() {
        if (!daqWorker) return;
        if (!daqWorker->isRunning()) {
            daqWorker->startAcquisition("Dev1/ai0", 1000.0, 200);
            realTimeBtn->setText("StopRealtime");
        } else {
            daqWorker->stopAcquisition();
            realTimeBtn->setText("RealTime");
        }
    });

    // Log group
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
    statusLabel = new QLabel("Status Log: <font color='red'>Disable</font>");
    logLayout->addWidget(statusLabel);
    logGroup->setLayout(logLayout);
    leftLayout->addWidget(logGroup);

    // Connect Log Single button to logging slot
    connect(logSingleBtn, &QPushButton::clicked, this, &MainWindow::logSensorData);

    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    QWidget *leftWidget = new QWidget;
    QFont controlFont("Arial", 9);
    leftWidget->setFont(controlFont);
    leftWidget->setLayout(leftLayout);
    leftWidget->setFixedWidth(200);
    mainLayout->addWidget(leftWidget);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 4);

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
    QSizePolicy plotPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    simPlot->setSizePolicy(plotPolicy);
    simPlot2->setSizePolicy(plotPolicy);
    simPlot3->setSizePolicy(plotPolicy);
    simPlot4->setSizePolicy(plotPolicy);

    simPlot->setMinimumHeight(360);
    simPlot2->setMinimumHeight(240);
    simPlot3->setMinimumHeight(360);
    simPlot4->setMinimumHeight(240);

    rightLayout->addWidget(simPlot, 3);
    rightLayout->addWidget(simPlot2, 2);
    rightLayout->addWidget(simPlot3, 3);
    rightLayout->addWidget(simPlot4, 2);

    QFont axisFont("Arial", 16, QFont::Bold);
    QFont tickFont("Arial", 12);
    auto applyAxisFonts = [&](QCustomPlot* p){
        p->xAxis->setLabelFont(axisFont);
        p->yAxis->setLabelFont(axisFont);
        p->xAxis->setTickLabelFont(tickFont);
        p->yAxis->setTickLabelFont(tickFont);
    };
    applyAxisFonts(simPlot);
    applyAxisFonts(simPlot2);
    applyAxisFonts(simPlot3);
    applyAxisFonts(simPlot4);

    mainLayout->addLayout(rightLayout);

}
void MainWindow::logSensorData() {
    QDir dir;
    if (!dir.exists("log")) {
        dir.mkdir("log");
    }
    QFile file("log/sensor_log.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Time,Value\n";
        for (int i = 0; i < tData.size() && i < yData.size(); ++i) {
            out << tData[i] << "," << yData[i] << "\n";
        }
        file.close();
        if (statusLabel) statusLabel->setText("Status Log: <font color='green'>Log thành công</font>");
    } else {
        if (statusLabel) statusLabel->setText("Status Log: <font color='red'>Log lỗi</font>");
    }
}
void MainWindow::updateDeviceStatus()
{
    QStringList devs = DAQWorker::availableDevices();
    if (devs.isEmpty()) {
        deviceStatusLabel->setText("<b>Device:</b> Not found");
        deviceStatusLabel->setStyleSheet("color: red;");
    } else {
        QString d = devs.first();
        deviceStatusLabel->setText(QString("<b>Device:</b> %1  — Connected").arg(d));
        deviceStatusLabel->setStyleSheet("color: green;");
    }
}

void MainWindow::setupSimulationPlot(QCustomPlot*& plot, QVector<double>& t, QVector<double>& y, int& idx, QColor color) {
    plot = new QCustomPlot;
    plot->addGraph();
    plot->xAxis->setLabel("Time [s]");
    plot->yAxis->setLabel("Amplitude");

    plot->graph(0)->setPen(QPen(color));
    // Performance: disable antialiasing and heavy drawing features for embedded devices
    plot->setNotAntialiasedElements(QCP::aeAll);
    plot->setNoAntialiasingOnDrag(true);
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    plot->graph(0)->setAdaptiveSampling(true);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // minimal interactions
    // Reserve buffers to avoid repeated reallocations
    t.reserve(20000);
    y.reserve(20000);
    t.clear();
    y.clear();
    idx = 0;
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

void MainWindow::onDaqSamples(const QVector<double> &times, const QVector<double> &values) {
    // Append incoming DAQ samples to plot buffer (for demo append to plot 1)
    // Keep buffer bounded
    const int maxSamples = 20000;
    for (int i = 0; i < values.size(); ++i) {
        tData.append(times[i]);
        yData.append(values[i]);
    }
    if (tData.size() > maxSamples) {
        int removeCnt = tData.size() - maxSamples;
        tData.remove(0, removeCnt);
        yData.remove(0, removeCnt);
    }

    // Tự động ghi log mỗi lần nhận dữ liệu mới
    QDir dir;
    if (!dir.exists("log")) {
        dir.mkdir("log");
    }
    QFile file("log/sensor_log.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Time,Value\n";
        for (int i = 0; i < tData.size() && i < yData.size(); ++i) {
            out << tData[i] << "," << yData[i] << "\n";
        }
        file.close();
    }
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
