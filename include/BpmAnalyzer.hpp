#pragma once

#include "AudioLoader.hpp"

// Estimate BPM using a simple onset/novelty curve and autocorrelation.
// Returns 0 on failure/insufficient data.
double estimateBPM(const AudioData& audio,
                   double minBpm = 80.0,
                   double maxBpm = 180.0);
