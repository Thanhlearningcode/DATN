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

    error = DAQmxCreateTask("", &task);
    if (error) goto done;

    error = DAQmxCreateAIVoltageChan(task, m_channel.toStdString().c_str(), "",
                                     DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, NULL);
    if (error) goto done;

    error = DAQmxCfgSampClkTiming(task, "", m_sampleRate, DAQmx_Val_Rising,
                                  DAQmx_Val_ContSamps, m_samplesPerRead);
    if (error) goto done;

    error = DAQmxStartTask(task);
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

done:
    if (task) {
        DAQmxStopTask(task);
        DAQmxClearTask(task);
    }
    if (error < 0) {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        qWarning() << "DAQmx Error:" << errBuff;
    }
#else
    // Stub mode: emit a sine wave for demo when NIDAQ is not available
    double t = 0;
    const double dt = 1.0 / m_sampleRate;
    while (m_running) {
        QVector<double> times(m_samplesPerRead), values(m_samplesPerRead);
        for (int i = 0; i < m_samplesPerRead; ++i) {
            times[i] = t;
            values[i] = std::sin(2.0 * M_PI * 10.0 * t);
            t += dt;
        }
        emit samplesReady(times, values);
        QThread::msleep( (int)(1000.0 * m_samplesPerRead / m_sampleRate) );
    }
#endif
}

QStringList DAQWorker::availableDevices() {
#ifdef NIDAQ_AVAILABLE
    // If NI-DAQ is available, query devices here. For now return empty to let runtime detect.
    // TODO: implement actual device enumeration using DAQmx functions when available.
    return QStringList();
#else
    // When NI drivers are not present, expose a simulated device name so UI can show 'Connected'
    return QStringList{"No Connect Dev"};
#endif
}
