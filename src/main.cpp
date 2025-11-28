#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void printUsage(const char* exeName) {
    std::cerr << "Usage: " << exeName << " <trackA> <trackB>\n";
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::vector<std::string> tracks{argv[1], argv[2]};

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
        std::cout << "Track " << (i == 0 ? "A" : "B") << ": " << path << " [found]\n";
    }

    std::cout << "Initialization complete. Analysis modules will be added in subsequent phases.\n";
    return 0;
}
