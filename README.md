# bela-sampler

Early-stage Bela Gem sampler. Loads WAV files from `samplesfolder/` and plays them in loop.

## Deploy (Bela)

```bash
./scripts/deploy.sh
```

Requires SSH access to the board (`root@bela.local` by default).

## Desktop dev (JUCE)

A local JUCE standalone lets you develop and debug without the Bela board. It reuses the same DSP core from `src/` and shows the same log output as `rt_printf` on Bela in a window.

### First-time setup

```bash
git submodule update --init
```

### Build with CMake

```bash
cd juce
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The app is built at `juce/build/BelaSampler_artefacts/Debug/BelaSampler.app`.

### Build with Projucer (alternative)

1. Open `juce/BelaSampler.jucer` in Projucer
2. Save and open the generated Xcode project in `juce/Builds/MacOSX/`
3. Build and run from Xcode

### Run

Launch the app from the **repository root** so it can find `samplesfolder/` and `program/`:

```bash
cd /path/to/bela-sampler
open juce/build/BelaSampler_artefacts/Debug/BelaSampler.app
```

Or set the working directory to the repo root in your IDE run configuration.

### What you get

- Audio output: master bus stereo (channels 0–1) at 44.1 kHz
- MIDI input: first available device auto-connected
- Log window: sample loading, program mapping, MIDI messages (same content as Bela terminal)

### Project layout

| Path | Role |
|------|------|
| `src/` | Shared sampler core (DSP, engine, programs) |
| `platform/` | Platform glue (logging, bootstrap) |
| `render.cpp` | Bela entry point |
| `juce/` | Desktop JUCE standalone |
| `external/JUCE/` | JUCE submodule |
