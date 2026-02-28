#pragma once

#include <juce_graphics/juce_graphics.h>

namespace bvs::Colours {

    static inline const juce::Colour vhsPurple = juce::Colour::fromFloatRGBA(0.55f, 0.20f, 0.85f, 1.f);

    static inline const juce::Colour vhsBlue   = juce::Colour::fromFloatRGBA(0.15f, 0.35f, 0.95f, 1.f);

    static inline const juce::Colour vhsCyan   = juce::Colour::fromFloatRGBA(0.10f, 0.95f, 0.95f, 1.f);

    static inline const juce::Colour vhsMagenta= juce::Colour::fromFloatRGBA(0.95f, 0.10f, 0.75f, 1.f);

    static inline const juce::Colour vhsDark   = juce::Colour::fromFloatRGBA(0.05f, 0.05f, 0.08f, 1.f);

    static inline const juce::Colour vhsGrid   = juce::Colour::fromFloatRGBA(0.20f, 0.25f, 0.35f, 1.f);

    juce::ColourGradient diagonalGradient(juce::Rectangle<float> bounds);

    void drawGlow(juce::Graphics& g, juce::Rectangle<float> r, juce::Colour c,

                  float strength = 0.8f, int passes = 3);

}
