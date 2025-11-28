#include "AudioLoader.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include <sndfile.h>

namespace {
float computeMaxAbs(const std::vector<float>& data) {
    float maxVal = 0.0f;
    for (float v : data) {
        maxVal = std::max(maxVal, std::abs(v));
    }
    return maxVal;
}
} // namespace

AudioData loadAudioFile(const std::string& path) {
    SF_INFO info{};
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &info);
    if (!file) {
        throw std::runtime_error("Failed to open audio file: " + path);
    }

    if (info.channels <= 0 || info.samplerate <= 0) {
        sf_close(file);
        throw std::runtime_error("Invalid audio metadata in file: " + path);
    }

    const int frames = static_cast<int>(info.frames);
    std::vector<float> interleaved(static_cast<size_t>(frames) * info.channels);
    sf_count_t readFrames = sf_readf_float(file, interleaved.data(), frames);
    sf_close(file);

    if (readFrames != frames) {
        throw std::runtime_error("Short read from file: " + path);
    }

    // Convert to mono by averaging channels if needed.
    std::vector<float> mono(static_cast<size_t>(frames));
    if (info.channels == 1) {
        mono = std::move(interleaved);
    } else {
        for (int i = 0; i < frames; ++i) {
            float sum = 0.0f;
            for (int ch = 0; ch < info.channels; ++ch) {
                sum += interleaved[static_cast<size_t>(i) * info.channels + ch];
            }
            mono[static_cast<size_t>(i)] = sum / static_cast<float>(info.channels);
        }
    }

    // Normalize to -0.99..0.99 peak to avoid clipping; preserves silence.
    float maxAbs = computeMaxAbs(mono);
    if (maxAbs > 0.000001f) {
        const float targetPeak = 0.99f;
        if (maxAbs > targetPeak) {
            const float gain = targetPeak / maxAbs;
            for (float& v : mono) {
                v *= gain;
            }
        }
    }

    AudioData out;
    out.samples = std::move(mono);
    out.sampleRate = info.samplerate;
    out.channels = info.channels;
    return out;
}
