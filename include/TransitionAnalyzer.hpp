#pragma once

#include "AnalysisTypes.hpp"

// Find the best transition window between two tracks based on BPM, key, and energy alignment.
// Fills component scores (0-1) and overall score (0-10).
TransitionSuggestion findBestTransition(const TrackAnalysis& a, const TrackAnalysis& b);
