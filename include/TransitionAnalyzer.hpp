#pragma once

#include "AnalysisTypes.hpp"

// Find the best transition window between two tracks based on BPM, key, and energy alignment.
TransitionSuggestion findBestTransition(const TrackAnalysis& a, const TrackAnalysis& b);
