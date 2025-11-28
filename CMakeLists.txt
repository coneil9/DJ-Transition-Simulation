cmake_minimum_required(VERSION 3.15)
project(DJTransitionSimulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External dependencies (to be filled in future phases).
# You can adapt these find_package calls to match your environment.
# find_package(libsndfile REQUIRED)
# find_package(FFTW3 REQUIRED)

add_executable(djtransition
    src/main.cpp
)

target_include_directories(djtransition
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Link against external libraries when added.
# target_link_libraries(djtransition PRIVATE sndfile FFTW3::fftw3)
