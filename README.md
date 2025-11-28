# 🎧 DJ Transition Simulator  
A C++ audio analysis tool that evaluates transition compatibility between two tracks.

---

##  Overview

**DJ Transition Simulator** is a C++ command-line application that analyzes two audio tracks and predicts **how well they can be mixed together**.  
It performs real audio-DSP analysis including **BPM detection**, **energy profiling**, **rough key estimation**, and computes **transition windows** where the tracks align best musically and energetically.

---

## 🚀 Features

###  Audio Analysis
- Load WAV audio files via `libsndfile`
- Normalize sample amplitudes
- Convert multi-channel audio to mono
- Extract sample rate, duration, and metadata

---

###  BPM Detection
Uses classic DSP techniques:
- Onset envelope generation
- Autocorrelation-based periodicity detection  
Produces an approximate **BPM** for each track.

---

###  Key Estimation
Approximate harmonic key detection via:
- FFT per analysis window  
- Pitch-class histogram construction  
- Matching to major/minor key templates  
Outputs results like **“A minor”** or **“8A (Camelot)”**.

---

### Energy Curve Extraction
- Computes RMS energy per fixed window (e.g., **0.50s**)
- Produces a time-series energy curve  
- Helps identify **drops**, **build-ups**, **breakdowns**, and **chorus sections**

---

###  Transition Compatibility Score
A heuristic scoring algorithm evaluates:
- **BPM similarity** (or feasible tempo stretch)
- **Harmonic key compatibility**
- **Energy alignment** between the two tracks
- General **musical smoothness** of transitioning from A → B

#### Output Scores:
- **Overall score:** 0–10  
- **Energy alignment score**  
- **Harmonic compatibility score**  
- **BPM match score**

---

###  Best Transition Window Detection
Suggests:
- **Where to mix out** of Track A  
- **Where to mix in** to Track B  
- Timestamps formatted as `mm:ss`

---

## Example Output

Track A: Doses-Mimosas.wav
Sample rate: 48000 Hz
Channels : 2 (converted to mono)
Frames : 16092824
Duration : 335.27 s (05:35)
BPM : 114.80
Energy : windows=671 windowSec=0.50
min=0.000000 max=0.272487
Key : C#/Db major

Track B: Way-To-Go.wav
Sample rate: 48000 Hz
Channels : 2 (converted to mono)
Frames : 9338736
Duration : 194.56 s (03:15)
BPM : 122.28
Energy : windows=390 windowSec=0.50
min=0.000000 max=0.364844
Key : F#/Gb major

=== Suggested transition ===
Mix out of Track A at 05:25 -> into Track B at 01:23
Compatibility score: 5.95 / 10
BPM component : 4.00 / 10
Key component : 8.50 / 10
Energy component: 6.00 / 10

Analysis completed.

markdown
Copy code

---

## 🧱 Technical Architecture

### 📦 Core Modules

#### **AudioLoader**
- Reads WAV files using `libsndfile`  
- Normalizes audio  
- Converts multi-channel → mono

#### **BpmAnalyzer**
- Builds onset strength envelope  
- Autocorrelation-based BPM estimator  

#### **EnergyAnalyzer**
- Computes RMS energy per window  
- Produces full energy curve vector  

#### **KeyAnalyzer** *(optional)*
- FFT → spectral profile → pitch-class histogram  
- Matches against major/minor key templates  

#### **TransitionScorer**
- Computes BPM, key, and energy alignment scores  
- Generates final compatibility score  
- Selects best transition timestamps  

#### **OutputFormatter**
- Clean, human-readable terminal output  
- (Optional) JSON export  

---

## 🛠 Installation & Setup

### 🔧 Prerequisites
- **C++17** or later  
- **CMake 3.10+**  
- **libsndfile** (audio file loading)  
- **fftw3** (FFT operations)  

---

### 🏗 Build Instructions

``` bash
git clone https://github.com/your-username/DJ-Transition-Simulator
cd DJ-Transition-Simulator
mkdir build && cd build
cmake -S . -B build
cmake --build build
▶️ Run the Program

``` 

# 
How the Scoring Works
BPM Compatibility
Perfect/near-perfect match → high score

Small stretchable difference → moderate

Large difference → penalty

Key Compatibility
Harmonic mixing rules:

Same key → excellent

Relative minor/major, fifths → smooth

Distant keys → poor

Energy Alignment
Ideal: low → building → drop

Avoid: drop into breakdown

Alignment uses RMS curve similarity

Final Score Formula
text
Copy code
finalScore = w1 * bpmScore 
           + w2 * keyScore 
           + w3 * energyScore
Where:

w1 + w2 + w3 = 1

Weights can be tuned as needed