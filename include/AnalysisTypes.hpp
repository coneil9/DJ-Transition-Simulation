#pragma once

#include <string>
#include <vector>

struct TrackAnalysis {
    double bpm = 0.0;
    std::string key = "Unknown";
    std::vector<double> energyCurve; // RMS per window
    double windowSeconds = 0.0;
};

struct TransitionSuggestion {
    double score = 0.0; // 0-10
    double timeA = 0.0; // seconds in Track A
    double timeB = 0.0; // seconds in Track B
};
