# BoratoVHSStyle

> A production-quality retro VHS UI theme library for JUCE 8 plugins and apps.

Neon glows · Scanlines · Animated grain · Segmented encoders · Chromatic aberration

---

## Table of Contents

- [Features](#features)
- [Components](#components)
  - [Knobs](#knobs)
  - [Controls](#controls)
  - [Backgrounds](#backgrounds)
- [Getting Started](#getting-started)
  - [CMake](#cmake)
  - [Projucer](#projucer)
- [Usage](#usage)
- [KnobShowcase Example](#knobshowcase-example)
- [License](#license)

---

## Features

| Category | What you get |
|---|---|
| **Knobs** | 4 rotary variants from vintage analog to segmented digital |
| **LookAndFeel** | Drop-in VHS theme for all standard JUCE controls |
| **Backgrounds** | Animated scanlines, grid, and dark film grain |
| **Audio Reactive** | Pulse halo driven by external audio level |
| **Performance** | Geometry cached — no sin/cos per frame, no heap allocs in `paint()` |
| **High DPI** | All drawing in logical pixels via `juce::AffineTransform` |
| **DAW ready** | No idle value drift, compatible with automation |
| **JUCE 8** | Targets JUCE 8.0.7+, C++17, no external dependencies |

---

## Components

### Knobs

All rotary controls share `KnobStyle` and the `KnobGeometry` cache layer.

```cpp
enum class KnobStyle { Basic, Pro, NeonRing, Digital };
```

#### `VHSKnob` — Basic

The original single-ring rotary. Lightweight, backward-compatible.

#### `VHSKnobPro` — Pro

Full-feature knob with radial-gradient body, progress arc, triangular pointer,
graduated ticks, inner shadow simulation, specular highlight, and dark micro-texture.

```cpp
bvs::VHSKnobPro knob;
knob.setRange (-48.0, 0.0, 0.1);
knob.setAccentColour (bvs::Colours::vhsMagenta);
knob.setMarkerCount  (11);
knob.setGlowStrength (0.65f);
knob.setRingThickness(4.f);
addAndMakeVisible (knob);
```

| Method | Description |
|---|---|
| `setGlowStrength(float)` | Overall neon glow intensity `[0, 1]` |
| `setMarkerCount(int)` | Tick mark count `[2, 24]` |
| `setAccentColour(Colour)` | Arc and pointer colour |
| `setRingThickness(float)` | Outer ring width in px |
| `setTextureBrightness(int, int)` | Body grain brightness range |
| `setTextureAlphaMax(float)` | Max body grain opacity |

#### `VHSKnobProClaro` — Pro (lighter texture variant)

Identical to `VHSKnobPro` with a more visible body grain (40–90 brightness, 2% alpha).
Useful on darker backgrounds where extra surface depth is desired.

```cpp
bvs::VHSKnobProClaro knob; // all VHSKnobPro methods available
```

#### `VHSKnobNeonRing` — Neon Ring

Minimalist dark body with a prominent three-pass glow arc, chromatic-aberration
pointer (separate cyan/magenta channels), and an audio-reactive pulse halo.

```cpp
bvs::VHSKnobNeonRing knob;
knob.setRingThickness (6.f);

// Drive the pulse halo from your audio processor (message thread only):
juce::MessageManager::callAsync ([this, rms]() mutable {
    knob.setPulseIntensity (rms);
});
```

#### `VHSKnobDigital` — Digital Encoder

Segmented ring with per-step colouring (dim / lit / leading-edge), a step-crossing
flash effect, and a cached monospace value label in the centre.

```cpp
bvs::VHSKnobDigital knob;
knob.setStepCount (16);
knob.setValueFormat ([] (double v) {
    return (v >= 0.0 ? "+" : "") + juce::String (v, 1) + " dB";
});
```

---

### Controls

| Class | Description |
|---|---|
| `VHSLookAndFeel` | Master LookAndFeel — apply once to theme all JUCE controls |
| `VHSSlider` | Linear slider with purple→blue gradient fill and pixel handle |
| `VHSButton` | Text button with hover glow and rounded corners |
| `VHSMeter` | Level meter with peak hold and cyan afterglow decay |

```cpp
// Apply the VHS theme to the entire plugin window
bvs::VHSLookAndFeel laf;
setLookAndFeel (&laf);

// Or use without VHSKnob* subclasses — set the style property on any slider:
mySlider.getProperties().set ("bvs_knobStyle", (int) bvs::KnobStyle::NeonRing);
```

---

### Backgrounds

| Class | Description |
|---|---|
| `VHSGridBackground` | Dark background with diagonal gradient and dim grid lines |
| `ScanlinesOverlay` | Animated horizontal scanlines (CRT/TV effect) |
| `VHSNoiseLayer` | Animated dark film grain — subtle, dark, VHS-accurate |

```cpp
bvs::VHSGridBackground  grid;
bvs::ScanlinesOverlay   scanlines;
bvs::VHSNoiseLayer      noise;

grid.setSpacing (24);
scanlines.setOpacity (0.12f);
scanlines.setSpeed   (0.5f);
noise.setAmount (0.06f); // 0 = off, 1 = heavy grain
noise.setFPS    (24);    // lower for cheaper CPU

addAndMakeVisible (grid);
addAndMakeVisible (scanlines);
addAndMakeVisible (noise);
```

> **Tip:** Layer them in this order — grid first (bottom), scanlines on top, noise last.

---

### Colour Palette

```cpp
namespace bvs::Colours {
    vhsPurple   // #8C33D9
    vhsBlue     // #2659F2
    vhsCyan     // #1AF2F2
    vhsMagenta  // #F21AC0
    vhsDark     // #0D0D14  ← background base
    vhsGrid     // #333347  ← grid lines

    // Utilities
    juce::ColourGradient diagonalGradient (juce::Rectangle<float> bounds);
    void drawGlow (juce::Graphics&, juce::Rectangle<float>, juce::Colour,
                   float strength = 0.8f, int passes = 3);
}
```

---

## Getting Started

### CMake

```bash
git clone https://github.com/borato/BoratoVHSStyle.git
cd BoratoVHSStyle
cmake -B build -DBVS_BUILD_SHOWCASE=ON
cmake --build build --config Release
```

In your own `CMakeLists.txt`:

```cmake
add_subdirectory(BoratoVHSStyle)

target_link_libraries(MyPlugin PRIVATE BoratoVHSStyle)
```

JUCE 8 is fetched automatically from GitHub if not found locally.
Set `-DBVS_FETCH_JUCE=OFF` and point `JUCE_DIR` to your local install to skip download.

### Projucer

1. Open Projucer and add the `borato_vhsstyle` module:
   **Modules → Add module from folder →** select `modules/`
2. Set the module's **Extra Header Search Paths** to the repo root `include/`.
3. Save and open in your IDE.

The `KnobShowcase.jucer` in `examples/KnobShowcase/` is a ready-to-run example
for Visual Studio 2022.

> **Note:** `JuceLibraryCode/` is auto-generated by Projucer and is excluded from
> version control. Re-save the `.jucer` to regenerate it.

---

## Usage

### Minimal Setup (existing plugin)

```cpp
// PluginEditor.h
#include <BoratoVHSStyle/BoratoVHSStyle.h>

class MyEditor : public juce::AudioProcessorEditor {
    bvs::VHSLookAndFeel laf;
    bvs::VHSKnobPro     gainKnob;
    bvs::VHSKnobDigital freqKnob;
    bvs::VHSGridBackground bg;
    bvs::ScanlinesOverlay  scanlines;
};
```

```cpp
// PluginEditor.cpp
MyEditor::MyEditor (MyProcessor& p) : AudioProcessorEditor (p)
{
    setOpaque (true);
    setLookAndFeel (&laf);

    gainKnob.setRange (-48.0, 6.0, 0.1);
    gainKnob.setAccentColour (bvs::Colours::vhsCyan);
    addAndMakeVisible (gainKnob);

    freqKnob.setRange (20.0, 20000.0);
    freqKnob.setStepCount (24);
    addAndMakeVisible (freqKnob);

    bg.setSpacing (20);
    scanlines.setOpacity (0.10f);
    addAndMakeVisible (bg);
    addAndMakeVisible (scanlines);

    setSize (600, 400);
}

void MyEditor::resized()
{
    bg.setBounds       (getLocalBounds());
    scanlines.setBounds(getLocalBounds());
    gainKnob.setBounds (40, 80, 120, 120);
    freqKnob.setBounds (200, 80, 120, 120);
}

void MyEditor::paint (juce::Graphics& g) { g.fillAll (bvs::Colours::vhsDark); }
```

---

## KnobShowcase Example

A standalone app demonstrating all three advanced knob variants side-by-side,
with VHSMeters driven at 60 fps and a live pulse-intensity slider for the NeonRing.

**Open in VS2022:**
```
examples/KnobShowcase/KnobShowcase.jucer
→ Projucer: Save and Open in IDE
→ Build & Run (Debug x64)
```

**Build with CMake:**
```bash
cmake -B build -DBVS_BUILD_SHOWCASE=ON
cmake --build build --config Debug
./build/examples/KnobShowcase/Debug/KnobShowcase.exe
```

---

## Architecture Notes

| Concept | Detail |
|---|---|
| `KnobGeometry` | All `sin`/`cos`, arc paths, and pointer paths computed once; `isDirty()` skips rebuild when nothing changed |
| `KnobStyle` property | LookAndFeel reads `slider.getProperties()["bvs_knobStyle"]` to dispatch per-style rendering — works on any plain `juce::Slider` |
| `getDefaultMetricsKind()` | Returns `legacy` for stable cross-platform text metrics (JUCE 8 breaking change) |
| Micro-texture | 64×64 ARGB image built with `Image::BitmapData` at resize time; clipped to ellipse with `reduceClipRegion` |

---

## License

[Add your license here]
