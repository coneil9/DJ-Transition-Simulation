#include "BpmAnalyzer.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {
// Build a simple energy-based novelty curve using half-wave rectified energy differences.
std::vector<double> computeNovelty(const AudioData& audio, int frameSize, int hopSize) {
    const size_t totalSamples = audio.samples.size();
    if (totalSamples < static_cast<size_t>(frameSize)) return {};

    std::vector<double> novelty;
    double prevEnergy = 0.0;
    for (size_t start = 0; start + frameSize <= totalSamples; start += hopSize) {
        double energy = 0.0;
        for (int i = 0; i < frameSize; ++i) {
            float v = audio.samples[start + static_cast<size_t>(i)];
            energy += static_cast<double>(v) * static_cast<double>(v);
        }
        double diff = energy - prevEnergy;
        novelty.push_back(diff > 0.0 ? diff : 0.0);
        prevEnergy = energy;
    }

    // Normalize novelty to zero-mean to help autocorrelation.
    if (!novelty.empty()) {
        double mean = 0.0;
        for (double v : novelty) mean += v;
        mean /= static_cast<double>(novelty.size());
        for (double& v : novelty) v -= mean;
    }
    return novelty;
}

double autocorrelationAtLag(const std::vector<double>& x, size_t lag) {
    double sum = 0.0;
    const size_t N = x.size();
    for (size_t i = 0; i + lag < N; ++i) {
        sum += x[i] * x[i + lag];
    }
    return sum;
}
} // namespace

double estimateBPM(const AudioData& audio, double minBpm, double maxBpm) {
    if (audio.sampleRate <= 0 || audio.samples.empty()) return 0.0;
    if (minBpm <= 0.0 || maxBpm <= 0.0 || minBpm >= maxBpm) return 0.0;

    // Parameters: short frame for onset sensitivity, overlap hop.
    const int frameSize = 1024;
    const int hopSize = 512;

    auto novelty = computeNovelty(audio, frameSize, hopSize);
    if (novelty.size() < 4) return 0.0;

    // Convert BPM bounds to lag bounds in novelty frames.
    const double hopSeconds = static_cast<double>(hopSize) / static_cast<double>(audio.sampleRate);
    const double minPeriod = 60.0 / maxBpm; // shortest period corresponds to max BPM
    const double maxPeriod = 60.0 / minBpm; // longest period corresponds to min BPM

    size_t minLag = static_cast<size_t>(std::max(1.0, std::floor(minPeriod / hopSeconds)));
    size_t maxLag = static_cast<size_t>(std::ceil(maxPeriod / hopSeconds));
    if (maxLag >= novelty.size()) {
        maxLag = novelty.size() - 1;
    }
    if (minLag >= maxLag) return 0.0;

    double bestScore = -1e18;
    size_t bestLag = minLag;
    for (size_t lag = minLag; lag <= maxLag; ++lag) {
        double score = autocorrelationAtLag(novelty, lag);
        if (score > bestScore) {
            bestScore = score;
            bestLag = lag;
        }
    }

    double periodSeconds = static_cast<double>(bestLag) * hopSeconds;
    if (periodSeconds <= 0.0) return 0.0;
    double bpm = 60.0 / periodSeconds;
    return bpm;
}
