#include "KeyAnalyzer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <string>
#include <vector>

namespace {
// Krumhansl-Schmuckler key profiles (major/minor), normalized to sum=1.
const std::array<double, 12> MAJOR_PROFILE = {
    6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88};
const std::array<double, 12> MINOR_PROFILE = {
    6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17};

const std::array<const char*, 12> NOTE_NAMES = {
    "C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};

double noteIndexFromFreq(double freq) {
    // MIDI note index (A4=69 at 440 Hz)
    return 69.0 + 12.0 * std::log2(freq / 440.0);
}

// Simple Hann window.
std::vector<double> makeHann(int N) {
    std::vector<double> w(N);
    for (int n = 0; n < N; ++n) {
        w[n] = 0.5 * (1.0 - std::cos(2.0 * M_PI * n / (N - 1)));
    }
    return w;
}

// Naive DFT magnitude for a single frame.
std::vector<double> magnitudeSpectrum(const std::vector<float>& frame) {
    const int N = static_cast<int>(frame.size());
    std::vector<double> mag(static_cast<size_t>(N / 2 + 1), 0.0);
    for (int k = 0; k <= N / 2; ++k) {
        std::complex<double> sum{0.0, 0.0};
        for (int n = 0; n < N; ++n) {
            double angle = -2.0 * M_PI * k * n / static_cast<double>(N);
            sum += std::complex<double>(frame[n] * std::cos(angle), frame[n] * std::sin(angle));
        }
        mag[static_cast<size_t>(k)] = std::abs(sum);
    }
    return mag;
}

std::array<double, 12> normalizeProfile(const std::array<double, 12>& profile) {
    std::array<double, 12> out{};
    double sum = 0.0;
    for (double v : profile) sum += v;
    if (sum <= 0.0) return out;
    for (size_t i = 0; i < 12; ++i) out[i] = profile[i] / sum;
    return out;
}

double correlate(const std::array<double, 12>& a, const std::array<double, 12>& b) {
    double s = 0.0;
    for (size_t i = 0; i < 12; ++i) s += a[i] * b[i];
    return s;
}
} // namespace

std::string estimateKey(const AudioData& audio) {
    if (audio.sampleRate <= 0 || audio.samples.empty()) return "Unknown";

    const int frameSize = 4096;
    const int hopSize = 2048;
    if (audio.samples.size() < static_cast<size_t>(frameSize)) return "Unknown";

    auto window = makeHann(frameSize);
    std::array<double, 12> histogram{};
    const size_t totalSamples = audio.samples.size();
    for (size_t start = 0; start + frameSize <= totalSamples; start += hopSize) {
        std::vector<float> frame(frameSize);
        for (int n = 0; n < frameSize; ++n) {
            frame[n] = static_cast<float>(audio.samples[start + static_cast<size_t>(n)] * window[n]);
        }
        auto mag = magnitudeSpectrum(frame);

        const double binHz = static_cast<double>(audio.sampleRate) / frameSize;
        for (size_t k = 1; k < mag.size(); ++k) { // skip DC
            double freq = binHz * static_cast<double>(k);
            if (freq < 30.0 || freq > 5000.0) continue; // ignore extremes
            double midi = noteIndexFromFreq(freq);
            int pc = static_cast<int>(std::lround(midi)) % 12;
            if (pc < 0) pc += 12;
            histogram[static_cast<size_t>(pc)] += mag[k];
        }
    }

    double histSum = 0.0;
    for (double v : histogram) histSum += v;
    if (histSum <= 0.0) return "Unknown";
    for (double& v : histogram) v /= histSum;

    auto majTemplate = normalizeProfile(MAJOR_PROFILE);
    auto minTemplate = normalizeProfile(MINOR_PROFILE);

    double bestScore = -1e18;
    int bestRoot = 0;
    bool bestIsMajor = true;

    for (int shift = 0; shift < 12; ++shift) {
        std::array<double, 12> rotated{};
        for (int i = 0; i < 12; ++i) {
            rotated[i] = histogram[static_cast<size_t>((i + shift) % 12)];
        }
        double sMaj = correlate(rotated, majTemplate);
        double sMin = correlate(rotated, minTemplate);
        if (sMaj > bestScore) {
            bestScore = sMaj;
            bestRoot = shift;
            bestIsMajor = true;
        }
        if (sMin > bestScore) {
            bestScore = sMin;
            bestRoot = shift;
            bestIsMajor = false;
        }
    }

    const char* note = NOTE_NAMES[static_cast<size_t>(bestRoot)];
    return std::string(note) + (bestIsMajor ? " major" : " minor");
}
