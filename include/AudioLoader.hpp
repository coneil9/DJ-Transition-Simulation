#pragma once

#include <string>
#include <vector>

struct AudioData {
    std::vector<float> samples; // mono, normalized
    int sampleRate = 0;
    int channels = 0;
};

// Load an audio file via libsndfile, convert to mono, normalize to avoid clipping.
// Throws std::runtime_error on failure.
AudioData loadAudioFile(const std::string& path);
