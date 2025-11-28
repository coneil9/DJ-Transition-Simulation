## 🎧 DJ Transition Simulator 

A C++ audio analysis tool that evaluates transition compatibility between two tracks.

## Overview

DJ Transition Simulator is a C++ command-line application that analyzes two audio tracks and predicts how well they can be mixed together.
It performs real audio-DSP analysis, BPM detection, energy profiling, and rough key estimation, and computes transition points where the tracks align best musically and energetically.

## 🚀 Features

Audio Analysis

Load WAV audio files and normalize them
Convert multi-channel audio to mono
Extract sample rate, duration, and metadata

# BPM Detection

Detect tempo via:
Onset envelope generation
Autocorrelation-based periodicity analysis
Produces an approximate BPM for each track

# Key Estimation

Compute FFTs over time windows
Build a pitch-class histogram
Match to major/minor key profiles
Output approximate harmonic key (e.g., “A minor” or “8A” Camelot style)

# Energy Curve Extraction

Compute RMS energy values over fixed windows (e.g., 0.5s)
Create a time-series energy profile for each track
Identify breakdowns, build-ups, drops, etc.

# Transition Compatibility Score
A heuristic scoring algorithm evaluates:
BPM similarity (or stretchability)
Harmonic key compatibility
Energy alignment between the two tracks
“Musical smoothness” of transitioning from A to B

# Outputs:

Overall score (0–10)
Energy alignment score
Harmonic compatibility score
BPM match score

# Best Transition Window Detection

Suggests:
Where to mix out of Track A
Where to mix into Track B
With timestamps formatted as mm:ss

# Example Output

Track A: Doses-Mimosas.wav
  Sample rate: 48000 Hz
  Channels   : 2 (converted to mono)
  Frames     : 16092824
  Duration   : 335.27 s (05:35)
  BPM        : 114.80
  Energy     : windows=671 windowSec=0.50
               min=0.000000 max=0.272487
  Key        : C#/Db major

Track B: Way-To-Go.wav
  Sample rate: 48000 Hz
  Channels   : 2 (converted to mono)
  Frames     : 9338736
  Duration   : 194.56 s (03:15)
  BPM        : 122.28
  Energy     : windows=390 windowSec=0.50
               min=0.000000 max=0.364844
  Key        : F#/Gb major

=== Suggested transition ===
  Mix out of Track A at 05:25 -> into Track B at 01:23
  Compatibility score: 5.95 / 10
    BPM component   : 4.00 / 10
    Key component   : 8.50 / 10
    Energy component: 6.00 / 10
Analysis completed.

🧱 Technical Architecture
📦 Core Modules

AudioLoader

Reads WAV files using libsndfile

Normalizes audio and converts to mono

BpmAnalyzer

Builds onset strength envelope

Autocorrelation-based BPM estimator

EnergyAnalyzer

Computes RMS energy per window

Produces track energy curve vector

KeyAnalyzer (optional)

FFT → spectral profile → pitch-class histogram

Match against major/minor templates

TransitionScorer

Heuristic compatibility scoring

Evaluates BPM, key, and energy mapping

Selects best transition time alignment

OutputFormatter

Human-readable output


# Installation & Setup
Prerequisites

C++17+

CMake 3.10+

libsndfile (audio file loading)

fftw3 (FFT operations)

Build Instructions
cd DJ-Transition-Simulator
mkdir build && cd build
cmake -S . -B build
cmake --build build

Run
./djtransition trackA.wav trackB.wav


## How the Scoring Works
BPM Compatibility

Perfect or near-perfect match → high score

Small difference but stretchable → moderate

Large difference → penalty

Key Compatibility

Uses harmonic mixing rules:

Same key → excellent

Relative minor/major, perfect fifth → very good

Distant keys → penalty

Energy Alignment

Compares the two RMS energy curves:

Smooth transitions from low → high energy score well

Clashing energy (drop into breakdown) scores poorly

Final Score
finalScore = w1*bpmScore + w2*keyScore + w3*energyScore

Where weights sum to 1 and can be tuned.