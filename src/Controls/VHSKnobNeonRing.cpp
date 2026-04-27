#include "BoratoVHSStyle/Controls/VHSKnobNeonRing.h"

#include "BoratoVHSStyle/Colours.h"

#include <cmath>

namespace bvs {

// ---------------------------------------------------------------------------
VHSKnobNeonRing::VHSKnobNeonRing (KnobStyle style)
    : style_ (style)
{
    setSliderStyle  (juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    getProperties().set ("bvs_knobStyle", static_cast<int> (style_));
}

// ---- Config ---------------------------------------------------------------
void VHSKnobNeonRing::setRingThickness  (float px) noexcept { ringThickness_  = juce::jlimit (2.f, 16.f, px);        repaint(); }
void VHSKnobNeonRing::setPulseIntensity (float lv) noexcept { pulseIntensity_ = juce::jlimit (0.f, 1.f, lv);         repaint(); }

// ---- Resize ---------------------------------------------------------------
void VHSKnobNeonRing::resized() { /* geometry will be rebuilt lazily in paint */ }

// ---- paint() --------------------------------------------------------------
void VHSKnobNeonRing::paint (juce::Graphics& g)
{
    const auto  bounds      = getLocalBounds().toFloat();
    const float pos         = static_cast<float> (valueToProportionOfLength (getValue()));
    const float rotaryStart = juce::MathConstants<float>::pi * 1.25f;
    const float rotaryEnd   = juce::MathConstants<float>::pi * 2.75f;

    if (geom_.isDirty (bounds, pos, ringThickness_))
        geom_.rebuild (bounds, pos, rotaryStart, rotaryEnd, ringThickness_);

    drawPulseHalo       (g);
    drawBackground      (g);
    drawSweepRing       (g);
    drawDualGlowArc     (g);
    drawChromaticPointer(g);
}

// ---- Pulse halo (audio-reactive outer glow) ---------------------------------
void VHSKnobNeonRing::drawPulseHalo (juce::Graphics& g) const
{
    if (pulseIntensity_ < 0.01f)
        return;

    const float haloR = geom_.radius * (1.12f + pulseIntensity_ * 0.16f);
    const auto&  c    = geom_.centre;

    juce::ColourGradient halo (
        Colours::vhsCyan.withAlpha (pulseIntensity_ * 0.16f),
        c.x, c.y,
        juce::Colours::transparentBlack,
        c.x + haloR, c.y,
        true);

    g.setGradientFill (halo);
    g.fillEllipse (c.x - haloR, c.y - haloR, haloR * 2.f, haloR * 2.f);
}

// ---- Dark body circle ----------------------------------------------------
void VHSKnobNeonRing::drawBackground (juce::Graphics& g) const
{
    const float r = geom_.radius - ringThickness_ * 0.5f;
    const auto& c = geom_.centre;

    const auto outer = juce::Rectangle<float> (c.x - r, c.y - r, r * 2.f, r * 2.f)
                           .expanded (ringThickness_ * 0.38f);

    g.setColour (juce::Colours::black.withAlpha (0.82f));
    g.fillEllipse (outer);

    juce::ColourGradient grad (
        Colours::vhsDark.brighter (0.10f), c.x, c.y - r * 0.35f,
        Colours::vhsDark.darker   (0.62f), c.x, c.y + r,
        true);

    g.setGradientFill (grad);
    g.fillEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f);

    // Subtle rim
    g.setColour (Colours::vhsGrid.withAlpha (0.34f));
    g.drawEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f, 0.8f);

    g.setColour (juce::Colours::white.withAlpha (0.06f));
    g.drawEllipse (c.x - r + 2.f, c.y - r + 2.f, r * 2.f - 4.f, r * 2.f - 4.f, 0.7f);
}

// ---- Full-sweep dim ring (shows total travel range) ----------------------
void VHSKnobNeonRing::drawSweepRing (juce::Graphics& g) const
{
    g.setColour (Colours::vhsGrid.withAlpha (0.22f));
    g.strokePath (geom_.sweepArc,
                  juce::PathStrokeType (ringThickness_ * 0.78f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
}

// ---- Dual-glow neon progress arc -----------------------------------------
void VHSKnobNeonRing::drawDualGlowArc (juce::Graphics& g) const
{
    if (geom_.progressArc.isEmpty())
        return;

    // Pass 1: wide outer glow (blue)
    g.setColour (Colours::vhsBlue.withAlpha (0.14f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 2.2f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // Pass 2: medium glow (cyan, slightly narrower)
    g.setColour (Colours::vhsCyan.withAlpha (0.32f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 1.20f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // Pass 3: tight bright core
    g.setColour (Colours::vhsCyan.withAlpha (0.82f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 0.42f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
}

// ---- Chromatic-aberration pointer ----------------------------------------
void VHSKnobNeonRing::drawChromaticPointer (juce::Graphics& g) const
{
    const auto tip    = geom_.pointerTip (0.66f);
    const auto centre = geom_.centre;

    const juce::PathStrokeType shadowStroke (4.2f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded);
    const juce::PathStrokeType lineStroke (1.8f, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded);

    {
        juce::Path p;
        p.startNewSubPath (centre.x, centre.y);
        p.lineTo           (tip.x,   tip.y);
        g.setColour (juce::Colours::black.withAlpha (0.55f));
        g.strokePath (p, shadowStroke);
    }

    // Magenta channel (shifted +1.5px X)
    {
        juce::Path p;
        p.startNewSubPath (centre.x + 0.8f, centre.y);
        p.lineTo           (tip.x + 0.8f,   tip.y);
        g.setColour (Colours::vhsMagenta.withAlpha (0.32f));
        g.strokePath (p, lineStroke);
    }

    // Cyan channel (centred)
    {
        juce::Path p;
        p.startNewSubPath (centre.x, centre.y);
        p.lineTo           (tip.x,   tip.y);
        g.setColour (Colours::vhsCyan.withAlpha (0.82f));
        g.strokePath (p, lineStroke);
    }

    // Tiny dot at tip for extra crispness
    g.setColour (juce::Colours::white.withAlpha (0.58f));
    g.fillEllipse (tip.x - 1.6f, tip.y - 1.6f, 3.2f, 3.2f);
}

} // namespace bvs
