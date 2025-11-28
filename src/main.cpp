#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AudioLoader.hpp"

namespace fs = std::filesystem;

void printUsage(const char* exeName) {
    std::cerr << "Usage: " << exeName << " <trackA> <trackB>\n";
}

double secondsFromSamples(size_t samples, int sampleRate) {
    if (sampleRate <= 0) return 0.0;
    return static_cast<double>(samples) / static_cast<double>(sampleRate);
}

std::string formatTime(double seconds) {
    if (seconds < 0) seconds = 0;
    int totalSec = static_cast<int>(seconds + 0.5); // round
    int mins = totalSec / 60;
    int secs = totalSec % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mins << ":"
        << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::vector<std::string> tracks{argv[1], argv[2]};

    try {
        for (size_t i = 0; i < tracks.size(); ++i) {
            const auto& path = tracks[i];
            if (!fs::exists(path)) {
                std::cerr << "Error: file does not exist: " << path << "\n";
                return 1;
            }
            if (!fs::is_regular_file(path)) {
                std::cerr << "Error: path is not a regular file: " << path << "\n";
                return 1;
            }

            AudioData audio = loadAudioFile(path);
            double durationSec = secondsFromSamples(audio.samples.size(), audio.sampleRate);

            std::cout << "Track " << (i == 0 ? "A" : "B") << ": " << path << "\n";
            std::cout << "  Sample rate: " << audio.sampleRate << " Hz\n";
            std::cout << "  Channels   : " << audio.channels << " (converted to mono)\n";
            std::cout << "  Frames     : " << audio.samples.size() << "\n";
            std::cout << "  Duration   : " << std::fixed << std::setprecision(2)
                      << durationSec << " s (" << formatTime(durationSec) << ")\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    std::cout << "Audio loading completed. Next phases will add analysis modules.\n";
    return 0;
}
