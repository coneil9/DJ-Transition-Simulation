#pragma once

#include "AudioLoader.hpp"

#include <vector>

// Compute RMS energy over fixed windows (seconds).
// Returns an empty vector on failure/invalid params.
std::vector<double> computeEnergyCurve(const AudioData& audio, double windowSeconds);
