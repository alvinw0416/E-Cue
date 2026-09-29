
# E-Cue
**A 4-band parametric EQ plugin (VST3) built with JUCE/C++.**

**Author**: Alvin Wang
Sep 2026 - Present

![Screenshot of E-Cue Plugin](assets/image.png)

## Overview
E-Cue is a 4-band parametric equalizer, each band having frequency, Q, and gain controls. I built this as a learning project to understand the interaction and communication between the GUI (editor) and audio (processor) threads, as well as the general JUCE framework.

## Features
- **Filter Type(s)** - Peak biquad filters on each band. Band 1 and 4 are toggleable into a high-pass/low-pass respectively.
- **Audition mode** - Toggleable mode. While a knob is held, the output is bandpassed around that band's frequency and Q, so you can hear the region the band is acting on. All four bands' effect still apply within the bandpass.
- **Global Enable** - Plugin enable toggle.

## Status
The GUI is currently a barebones layout using JUCE's default components and look; a custom GUI is planned.

## Building
Requirements:
- CMake 3.23+
- Visual Studio 2022 with the "Desktop development with C++" workload
- Git and an internet connection (JUCE 8.0.14 is downloaded automatically by CMake on the first configure)

Build:
```
git clone [REPO_URL]
cd E-Cue
cmake -B build
cmake --build build --config Release
```
The first build will probably take a while because it has to fetch JUCE.

## Project Layout
- `source/Processor.*`: DSP, parameters, audition logic
- `source/Editor.*`, `source/LabeledKnob.h`: GUI