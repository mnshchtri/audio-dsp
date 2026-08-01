# Audio DSP Standalone JUCE App

This project is a pure C++ standalone synth application built with JUCE.

## Structure

- `cpp/` — C++ synth engine implementation
- `juce/` — JUCE application wrapper and UI

## Goals

- implement a simple synth oscillator and envelope in C++
- run as a standalone JUCE desktop application
- provide GUI controls for frequency, gain, waveform, and envelope

## Getting started

### 1. Install dependencies

- CMake 3.16+
- A C++17 compiler (Clang/GCC/MSVC)
- JUCE 7.x installed as a CMake package

### 2. Build C++ engine

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

### 3. Build JUCE standalone app

```bash
mkdir -p build && cd build
cmake -DJUCE_DIR=/path/to/JUCE ..
cmake --build .
```

If your JUCE installation is a CMake package, `JUCE_DIR` must point to the directory containing `JUCEConfig.cmake`.

For example:

```bash
cmake -DJUCE_DIR=/Users/mnsh/Developer/JUCE/cmake/juce ..
```

If that still fails, use:

```bash
cmake -DCMAKE_PREFIX_PATH=/Users/mnsh/Developer/JUCE/cmake/juce ..
```

## Notes

- The React UI has been removed in favor of a pure C++ JUCE app.
- The plugin wrapper was replaced with a standalone application.


The error you saw is because CMake could not locate the JUCE package, so it failed before creating `CMakeCache.txt`.

## Notes

This scaffold provides a simple synth engine in C++. It does not include a full VST host or plugin framework.
For a production plugin, integrate this code with a plugin framework like JUCE, iPlug2, or the Steinberg VST3 SDK.

## Next steps

1. Add host/plugin wrapper using VST3 or JUCE
2. Expose parameters to the UI via a bridge layer
3. Render the built React app in the plugin's UI view
