cmake_minimum_required(VERSION 3.15)
project(DJTransitionSimulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies (adjust to your environment).
# libsndfile is required for audio loading.
find_package(SndFile REQUIRED)
# find_package(FFTW3 REQUIRED)

add_executable(djtransition
    src/main.cpp
    src/AudioLoader.cpp
    src/BpmAnalyzer.cpp
    src/EnergyAnalyzer.cpp
    src/KeyAnalyzer.cpp
    src/TransitionAnalyzer.cpp
)

target_include_directories(djtransition
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Link against external libraries when added.
target_link_libraries(djtransition PRIVATE SndFile::sndfile)
# target_link_libraries(djtransition PRIVATE FFTW3::fftw3)
