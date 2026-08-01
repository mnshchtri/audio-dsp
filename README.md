# Audio DSP Guitar Rig

A standalone JUCE desktop app that processes live guitar (or any line/mic)
input through a configurable pedalboard, styled like a vintage stompbox rig.

## What it does

- Registers directly as an audio device callback and processes real input
  audio with minimal latency — pick your interface in-app via **Audio
  Settings...**
- Signal chain is a dynamic, user-built pedalboard: add pedals from the
  effect browser, drag to reorder in the chain strip, click a pedal's
  footswitch to bypass it, click a card to open its full controls
- A preset dropdown in the header loads a full rig (every pedal, in order,
  every knob) in one click — 5 tone-inspired presets ship out of the box
  (see `cpp/include/PresetLibrary.h`); these are fan tone-matches built from
  published gear interviews, not official/licensed artist presets
- 14 effect types across 6 categories:
  - **Dynamics** — Noise Gate, Compressor
  - **Drive** — Boost, Overdrive, Tube Screamer, Distortion, Fuzz
  - **Amp** — Amp (preamp + tone stack), Cabinet sim (EQ-curve based, no IRs)
  - **Modulation** — Chorus, Phaser, Tremolo
  - **Time** — Delay
  - **Reverb** — Reverb (Schroeder-style comb/allpass)
- UI is drawn as hammertone-textured pedal enclosures with knurled knobs,
  chrome footswitches, and LED jewels — all original procedural vector art
  (gradients/paths/generated noise), no scanned or scraped artwork

## Structure

- `cpp/` — portable C++17 DSP engine (no JUCE dependency): each effect is a
  self-contained class exposing `getParameters()` for reflection, wired
  together by `GuitarRigEngine` / `EffectChain` / `EffectFactory`
- `juce/` — JUCE application: audio I/O, the pedalboard UI (`ChainStripComponent`,
  `EffectBrowserPanel`, `ModuleEditorPanel`, `PedalCard`), and the custom
  `RigLookAndFeel` / `PedalTexture` vintage styling

## Getting started

### Dependencies

- CMake 3.16+
- A C++17 compiler (Clang/GCC/MSVC)
- [JUCE](https://github.com/juce-framework/JUCE) checked out locally (JUCE 8/9
  recommended — older releases may not build against current macOS SDKs)

### Build

```bash
git clone --depth 1 --branch 9.0.0 https://github.com/juce-framework/JUCE.git ~/Developer/JUCE

mkdir -p build && cd build
cmake -DJUCE_DIR=~/Developer/JUCE ..
cmake --build . --target AudioDSPApp
```

`JUCE_DIR` must point at your JUCE checkout (the directory containing JUCE's
own top-level `CMakeLists.txt`); it's pulled in via `add_subdirectory`, not
`find_package`, so no separate JUCE install/build step is needed.

The built app lands at:

```
build/juce/AudioDSPApp_artefacts/Debug/Audio DSP Guitar Rig.app
```

### Run

Launch the app, click **Audio Settings...** to pick your input interface and
output device, then play. macOS will prompt for microphone/input permission
on first launch.

## Known limitations

- Cabinet sim is an EQ-curve approximation, not real impulse-response
  convolution
- Presets are a fixed, built-in library — no saving your own custom chain as
  a new preset yet, and no persistence between launches (chain resets to a
  default 8-pedal layout on every launch unless you pick a preset again)
- Parameter updates from the UI to the audio thread aren't atomic-wrapped;
  fine for knob turns, not hardened against worst-case tearing
