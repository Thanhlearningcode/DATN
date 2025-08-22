#include "Simulation.h"
#include <cmath>
#include <random>

std::vector<double> Simulation::generateTwinSignal(double fs, double duration, double f1, double f2, double noiseLevel) {
    size_t N = static_cast<size_t>(fs * duration);
    std::vector<double> signal(N);
    std::default_random_engine gen;
    std::normal_distribution<double> noise(0.0, noiseLevel);
    for (size_t i = 0; i < N; ++i) {
        double t = i / fs;
        signal[i] = std::sin(2 * M_PI * f1 * t) + std::sin(2 * M_PI * f2 * t) + noise(gen);
    }
    return signal;
}

// FFT: placeholder, cần dùng thư viện ngoài như kissfft hoặc fftw
std::vector<std::complex<double>> Simulation::computeFFT(const std::vector<double>& signal) {
    // TODO: Tích hợp thư viện FFT thực tế
    return std::vector<std::complex<double>>();
}
