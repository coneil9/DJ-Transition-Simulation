#include <algorithm>
#include <array>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AnalysisTypes.hpp"
#include "AudioLoader.hpp"
#include "BpmAnalyzer.hpp"
#include "EnergyAnalyzer.hpp"
#include "KeyAnalyzer.hpp"
#include "TransitionAnalyzer.hpp"

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
    std::array<TrackAnalysis, 2> analyses{};

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

            TrackAnalysis analysis;
            analysis.bpm = estimateBPM(audio);
            analysis.windowSeconds = 0.5; // default energy window size in seconds
            analysis.energyCurve = computeEnergyCurve(audio, analysis.windowSeconds);
            analysis.key = estimateKey(audio);

            if (analysis.bpm <= 0.0) {
                std::cout << "  BPM        : (could not estimate)\n";
            } else {
                std::cout << "  BPM        : " << std::fixed << std::setprecision(2) << analysis.bpm << "\n";
            }

            if (analysis.energyCurve.empty()) {
                std::cout << "  Energy     : (could not compute)\n";
            } else {
                auto [minIt, maxIt] = std::minmax_element(analysis.energyCurve.begin(), analysis.energyCurve.end());
                std::cout << "  Energy     : windows=" << analysis.energyCurve.size()
                          << " windowSec=" << analysis.windowSeconds << "\n";
                std::cout << "               min=" << std::fixed << std::setprecision(6) << *minIt
                          << " max=" << std::fixed << std::setprecision(6) << *maxIt << "\n";
            }

            std::cout << "  Key        : " << analysis.key << "\n";
            analyses[i] = analysis;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    if (!analyses[0].energyCurve.empty() && !analyses[1].energyCurve.empty()) {
        TransitionSuggestion suggestion = findBestTransition(analyses[0], analyses[1]);
        std::cout << "\n=== Suggested transition ===\n";
        std::cout << "  Mix out of Track A at " << formatTime(suggestion.timeA)
                  << " -> into Track B at " << formatTime(suggestion.timeB) << "\n";
        std::cout << "  Compatibility score: " << std::fixed << std::setprecision(2)
                  << suggestion.score << " / 10\n";
        std::cout << "    BPM component   : " << std::fixed << std::setprecision(2)
                  << suggestion.bpmComponent * 10.0 << " / 10\n";
        std::cout << "    Key component   : " << std::fixed << std::setprecision(2)
                  << suggestion.keyComponent * 10.0 << " / 10\n";
        std::cout << "    Energy component: " << std::fixed << std::setprecision(2)
                  << suggestion.energyComponent * 10.0 << " / 10\n";
    }

    std::cout << "Analysis completed.\n";
    return 0;
}
