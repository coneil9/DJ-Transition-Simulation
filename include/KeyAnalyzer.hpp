#pragma once

#include "AudioLoader.hpp"

#include <string>

// Rough key estimation via pitch-class histogram against major/minor templates.
// Returns a string like "C major" or "A minor". Falls back to "Unknown".
std::string estimateKey(const AudioData& audio);
