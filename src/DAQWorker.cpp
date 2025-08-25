#include "DAQWorker.h"
#include <QDebug>
#include <QStringList>
#include <cmath>

// If NIDAQ is not available at compile time, provide a stub that emits zeros
#ifdef NIDAQ_AVAILABLE
extern "C" {
#include "NIDAQmx.h"
}
#endif

DAQWorker::DAQWorker(QObject *parent) : QThread(parent) {}

DAQWorker::~DAQWorker() { stopAcquisition(); wait(); }

void DAQWorker::startAcquisition(const QString &deviceChannel, double sampleRate, int samplesPerRead) {
    m_channel = deviceChannel;
    m_sampleRate = sampleRate;
    m_samplesPerRead = samplesPerRead;
    if (!isRunning()) {
        m_running = true;
        start();
    }
}

void DAQWorker::stopAcquisition() { m_running = false; }

void DAQWorker::run() {
#ifdef NIDAQ_AVAILABLE
    TaskHandle task = 0;
    int32 error = 0;
    char errBuff[2048] = {0};

    qDebug() << "[DAQWorker] Khởi tạo với channel:" << m_channel << ", sampleRate:" << m_sampleRate << ", samplesPerRead:" << m_samplesPerRead;

    error = DAQmxCreateTask("", &task);
    qDebug() << "[DAQWorker] DAQmxCreateTask error:" << error;
    if (error) goto done;

    error = DAQmxCreateAIVoltageChan(task, m_channel.toStdString().c_str(), "",
                                     DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, NULL);
    qDebug() << "[DAQWorker] DAQmxCreateAIVoltageChan error:" << error;
    if (error) goto done;

    error = DAQmxCfgSampClkTiming(task, "", m_sampleRate, DAQmx_Val_Rising,
                                  DAQmx_Val_ContSamps, m_samplesPerRead);
    qDebug() << "[DAQWorker] DAQmxCfgSampClkTiming error:" << error;
    if (error) goto done;

    error = DAQmxStartTask(task);
    qDebug() << "[DAQWorker] DAQmxStartTask error:" << error;
    if (error) goto done;

    std::vector<double> data(m_samplesPerRead);
    double sampleIndex = 0;
    while (m_running) {
        int32 samplesRead = 0;
        error = DAQmxReadAnalogF64(task, m_samplesPerRead, 10.0, DAQmx_Val_GroupByChannel,
                                   data.data(), m_samplesPerRead, &samplesRead, NULL);
        if (error < 0) {
            qWarning() << "DAQmx read error" << error;
            goto done;
        }
        QVector<double> times(samplesRead), values(samplesRead);
        for (int i = 0; i < samplesRead; ++i) {
            times[i] = sampleIndex / m_sampleRate;
            values[i] = data[i];
            sampleIndex += 1.0;
        }

        emit samplesReady(times, values);
    }

    if (task) {
        DAQmxStopTask(task);
        DAQmxClearTask(task);
    }
    if (error < 0) {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        qWarning() << "DAQmx Error:" << errBuff;
    }
#endif
}

QStringList DAQWorker::availableDevices() {
#ifdef NIDAQ_AVAILABLE
    // Query NI-DAQmx for available devices
    char buffer[4096] = {0};
    int32 bufferSize = sizeof(buffer);
    int32 error = DAQmxGetSysDevNames(buffer, bufferSize);
    qDebug() << "[DAQWorker] DAQmxGetSysDevNames error:" << error << ", buffer:" << buffer;
    QStringList devices;
    if (error == 0 && strlen(buffer) > 0) {
        // buffer contains device names separated by commas
        QString devStr = QString::fromLocal8Bit(buffer);
        devices = devStr.split(",", Qt::SkipEmptyParts);
        for (QString &dev : devices) dev = dev.trimmed();
    } else {
        qWarning() << "[DAQWorker] Không tìm thấy thiết bị DAQ hoặc lỗi truy vấn.";
    }
    return devices;
#else
    // When NI drivers are not present, expose a simulated device name so UI can show 'Connected'
    return QStringList{"No Connect Dev"};
#endif
}
