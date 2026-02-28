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

    const float haloR = geom_.radius * (1.18f + pulseIntensity_ * 0.22f);
    const auto&  c    = geom_.centre;

    juce::ColourGradient halo (
        Colours::vhsBlue.withAlpha (pulseIntensity_ * 0.35f),
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

    juce::ColourGradient grad (
        Colours::vhsDark.brighter (0.08f), c.x, c.y,
        Colours::vhsDark.darker   (0.35f), c.x, c.y + r,
        true);

    g.setGradientFill (grad);
    g.fillEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f);

    // Subtle rim
    g.setColour (Colours::vhsGrid.withAlpha (0.25f));
    g.drawEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f, 0.8f);
}

// ---- Full-sweep dim ring (shows total travel range) ----------------------
void VHSKnobNeonRing::drawSweepRing (juce::Graphics& g) const
{
    g.setColour (Colours::vhsGrid.withAlpha (0.30f));
    g.strokePath (geom_.sweepArc,
                  juce::PathStrokeType (ringThickness_,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
}

// ---- Dual-glow neon progress arc -----------------------------------------
void VHSKnobNeonRing::drawDualGlowArc (juce::Graphics& g) const
{
    if (geom_.progressArc.isEmpty())
        return;

    // Pass 1: wide outer glow (blue)
    g.setColour (Colours::vhsBlue.withAlpha (0.28f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 2.8f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // Pass 2: medium glow (cyan, slightly narrower)
    g.setColour (Colours::vhsCyan.withAlpha (0.45f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 1.55f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // Pass 3: tight bright core
    g.setColour (Colours::vhsCyan.withAlpha (0.92f));
    g.strokePath (geom_.progressArc,
                  juce::PathStrokeType (ringThickness_ * 0.55f,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
}

// ---- Chromatic-aberration pointer ----------------------------------------
void VHSKnobNeonRing::drawChromaticPointer (juce::Graphics& g) const
{
    const auto tip    = geom_.pointerTip (0.68f);
    const auto centre = geom_.centre;

    const juce::PathStrokeType lineStroke (2.2f, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded);

    // Magenta channel (shifted +1.5px X)
    {
        juce::Path p;
        p.startNewSubPath (centre.x + 1.5f, centre.y);
        p.lineTo           (tip.x + 1.5f,   tip.y);
        g.setColour (Colours::vhsMagenta.withAlpha (0.65f));
        g.strokePath (p, lineStroke);
    }

    // Cyan channel (centred)
    {
        juce::Path p;
        p.startNewSubPath (centre.x, centre.y);
        p.lineTo           (tip.x,   tip.y);
        g.setColour (Colours::vhsCyan);
        g.strokePath (p, lineStroke);
    }

    // Tiny dot at tip for extra crispness
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.fillEllipse (tip.x - 2.f, tip.y - 2.f, 4.f, 4.f);
}

} // namespace bvs
