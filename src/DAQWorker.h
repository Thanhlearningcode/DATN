#pragma once

#include <QThread>
#include <QVector>
#include <atomic>

class DAQWorker : public QThread {
    Q_OBJECT
public:
    explicit DAQWorker(QObject *parent = nullptr);
    ~DAQWorker() override;

    void startAcquisition(const QString &deviceChannel, double sampleRate, int samplesPerRead);
    void stopAcquisition();
    // Return available device names from NI driver (or simulated entry if unavailable)
    static QStringList availableDevices();

signals:
    void samplesReady(const QVector<double> &times, const QVector<double> &values);

protected:
    void run() override;

private:
    std::atomic<bool> m_running{false};
    QString m_channel;
    double m_sampleRate{1000.0};
    int m_samplesPerRead{100};
};
