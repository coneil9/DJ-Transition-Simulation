#include "TransitionAnalyzer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace {
double clamp01(double v) { return std::max(0.0, std::min(1.0, v)); }

double bpmCompatibility(double bpmA, double bpmB) {
    if (bpmA <= 0.0 || bpmB <= 0.0) return 0.5; // unknown; neutral
    double avg = 0.5 * (bpmA + bpmB);
    if (avg <= 0.0) return 0.0;
    double relDiff = std::abs(bpmA - bpmB) / avg; // relative mismatch
    if (relDiff <= 0.03) return 1.0;   // within ~3%
    if (relDiff <= 0.06) return 0.7;   // small stretch
    if (relDiff <= 0.10) return 0.4;   // moderate stretch
    return 0.15;                       // large mismatch
}

int pitchClassFromKeyString(const std::string& key) {
    // Expect formats like "C major", "A minor", "F#/Gb major".
    // Returns [-1] if unknown.
    if (key.empty() || key == "Unknown") return -1;
    std::string up = key;
    std::transform(up.begin(), up.end(), up.begin(), [](unsigned char c) { return std::toupper(c); });

    const std::array<std::pair<const char*, int>, 12> mapping = {{
        {"C", 0}, {"C#/DB", 1}, {"D", 2}, {"D#/EB", 3}, {"E", 4}, {"F", 5},
        {"F#/GB", 6}, {"G", 7}, {"G#/AB", 8}, {"A", 9}, {"A#/BB", 10}, {"B", 11},
    }};
    for (const auto& m : mapping) {
        if (up.rfind(m.first, 0) == 0) { // starts with note name
            return m.second;
        }
    }
    return -1;
}

double keyCompatibility(const std::string& keyA, const std::string& keyB) {
    int pcA = pitchClassFromKeyString(keyA);
    int pcB = pitchClassFromKeyString(keyB);
    if (pcA < 0 || pcB < 0) return 0.5; // unknown; neutral
    int diff = std::abs(pcA - pcB) % 12;
    // Same key
    if (diff == 0) return 1.0;
    // Perfect fifth or fourth (±7 or ±5) approximated by diff 5 or 7
    if (diff == 5 || diff == 7) return 0.85;
    // Relative major/minor (±3 semitones)
    if (diff == 3 || diff == 9) return 0.75;
    // Tritone clash
    if (diff == 6) return 0.2;
    // Generic neighbor
    if (diff == 1 || diff == 11) return 0.6;
    return 0.4;
}

std::vector<double> normalizeMinMax(const std::vector<double>& v) {
    if (v.empty()) return {};
    auto [mnIt, mxIt] = std::minmax_element(v.begin(), v.end());
    double mn = *mnIt;
    double mx = *mxIt;
    if (mx - mn < 1e-12) return std::vector<double>(v.size(), 0.0);
    std::vector<double> out(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        out[i] = (v[i] - mn) / (mx - mn);
    }
    return out;
}
} // namespace

TransitionSuggestion findBestTransition(const TrackAnalysis& a, const TrackAnalysis& b) {
    TransitionSuggestion best{};
    if (a.energyCurve.empty() || b.energyCurve.empty() || a.windowSeconds <= 0.0 || b.windowSeconds <= 0.0) {
        best.score = 0.0;
        return best;
    }

    const double bpmScore = bpmCompatibility(a.bpm, b.bpm);
    const double keyScore = keyCompatibility(a.key, b.key);

    auto normA = normalizeMinMax(a.energyCurve);
    auto normB = normalizeMinMax(b.energyCurve);

    double bestEnergyScore = -1.0;
    size_t bestIdxA = 0;
    size_t bestIdxB = 0;

    // Brute-force over window indices; for efficiency we could stride, but keep simple for clarity.
    for (size_t i = 0; i < normA.size(); ++i) {
        double curA = normA[i];
        double slopeA = (i > 0) ? (curA - normA[i - 1]) : 0.0;
        double lowA = 1.0 - curA; // prefer exiting on low energy
        for (size_t j = 0; j < normB.size(); ++j) {
            double curB = normB[j];
            double slopeB = (j > 0) ? (curB - normB[j - 1]) : 0.0;
            // Prefer entering on rising energy, or on higher energy than exit point.
            double riseComponent = clamp01(std::max(0.0, -slopeA) * std::max(0.0, slopeB));
            double levelComponent = clamp01(lowA * curB);
            double energyScore = 0.6 * levelComponent + 0.4 * riseComponent; // 0..1
            if (energyScore > bestEnergyScore) {
                bestEnergyScore = energyScore;
                bestIdxA = i;
                bestIdxB = j;
            }
        }
    }

    bestEnergyScore = clamp01(bestEnergyScore);

    // Combine components; weighted sum mapped to 0-10.
    double total01 = 0.4 * bpmScore + 0.3 * keyScore + 0.3 * bestEnergyScore;
    best.score = clamp01(total01) * 10.0;
    best.timeA = static_cast<double>(bestIdxA) * a.windowSeconds;
    best.timeB = static_cast<double>(bestIdxB) * b.windowSeconds;
    best.bpmComponent = bpmScore;
    best.keyComponent = keyScore;
    best.energyComponent = bestEnergyScore;
    return best;
}
