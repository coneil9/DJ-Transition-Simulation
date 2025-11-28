#include "EnergyAnalyzer.hpp"

#include <cmath>
#include <vector>

std::vector<double> computeEnergyCurve(const AudioData& audio, double windowSeconds) {
    if (audio.sampleRate <= 0 || audio.samples.empty() || windowSeconds <= 0.0) {
        return {};
    }
    const int windowSamples = static_cast<int>(std::round(windowSeconds * audio.sampleRate));
    if (windowSamples <= 0) return {};

    std::vector<double> curve;
    const size_t total = audio.samples.size();
    for (size_t start = 0; start < total; start += static_cast<size_t>(windowSamples)) {
        size_t end = std::min(total, start + static_cast<size_t>(windowSamples));
        double sumSq = 0.0;
        size_t count = end - start;
        for (size_t i = start; i < end; ++i) {
            double v = audio.samples[i];
            sumSq += v * v;
        }
        if (count == 0) continue;
        double rms = std::sqrt(sumSq / static_cast<double>(count));
        curve.push_back(rms);
    }
    return curve;
}
