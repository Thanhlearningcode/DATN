#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <complex>

class Simulation {
public:
    // Sinh tín hiệu dao động twins
    static std::vector<double> generateTwinSignal(double fs, double duration, double f1, double f2, double noiseLevel = 0.2);
    // FFT (sử dụng thư viện ngoài, ví dụ kissfft hoặc fftw)
    static std::vector<std::complex<double>> computeFFT(const std::vector<double>& signal);
};

#endif // SIMULATION_H
