#include "BoratoVHSStyle/Controls/VHSLookAndFeel.h"

#include "BoratoVHSStyle/Colours.h"
#include "BoratoVHSStyle/Fonts.h"

#include <cmath>

namespace bvs {

// ---------------------------------------------------------------------------
VHSLookAndFeel::VHSLookAndFeel()
{
    setColour (juce::Slider::thumbColourId,         Colours::vhsCyan);
    setColour (juce::Slider::trackColourId,         Colours::vhsBlue);
    setColour (juce::TextButton::buttonColourId,    Colours::vhsPurple);
    setColour (juce::TextButton::textColourOffId,   juce::Colours::white);
}

// ---------------------------------------------------------------------------
juce::Typeface::Ptr VHSLookAndFeel::getTypefaceForFont (const juce::Font& f)
{
    auto base = (f.getTypefaceName() == juce::Font::getDefaultMonospacedFontName())
        ? Fonts::body    (f.getHeight())
        : Fonts::heading (f.getHeight());

    return base.getTypefacePtr();
}

// ===========================================================================
// drawRotarySlider — dispatches by "bvs_knobStyle" slider property
// ===========================================================================
void VHSLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                        int x, int y, int w, int h,
                                        float sliderPos,
                                        float rotaryStart, float rotaryEnd,
                                        juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> ((float)x, (float)y,
                                                 (float)w, (float)h);

    const float ringThickness = 3.0f;

    if (lafGeom_.isDirty (bounds, sliderPos, ringThickness))
        lafGeom_.rebuild (bounds, sliderPos, rotaryStart, rotaryEnd, ringThickness);

    // Determine style from slider property (default = Basic)
    auto styleVar = slider.getProperties()["bvs_knobStyle"];
    const KnobStyle style = styleVar.isVoid()
        ? KnobStyle::Basic
        : static_cast<KnobStyle> (static_cast<int> (styleVar));

    switch (style)
    {
        case KnobStyle::Pro:
            drawProKnob      (g, lafGeom_, slider);
            break;

        case KnobStyle::NeonRing:
            drawNeonRingKnob (g, lafGeom_, slider);
            break;

        case KnobStyle::Digital:
        {
            auto stepVar  = slider.getProperties()["bvs_stepCount"];
            const int steps = stepVar.isVoid() ? 16 : static_cast<int> (stepVar);
            drawDigitalKnob (g, lafGeom_, sliderPos, steps, slider);
            break;
        }

        default:
            drawBasicKnob (g, lafGeom_, slider);
            break;
    }
}

// ===========================================================================
// Basic knob (original renderer — unchanged for backward compatibility)
// ===========================================================================
void VHSLookAndFeel::drawBasicKnob (juce::Graphics& g,
                                     const KnobGeometry& geom,
                                     juce::Slider&) const
{
    const float r = geom.radius;
    const auto& c = geom.centre;

    // Base ring
    g.setColour (Colours::vhsDark.brighter (0.2f));
    g.drawEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f, 2.f);

    // Glow
    Colours::drawGlow (g, { c.x - r, c.y - r, r * 2.f, r * 2.f },
                       Colours::vhsBlue, 0.28f, 1);

    // Ticks
    g.setColour (Colours::vhsGrid.brighter (0.65f).withAlpha (0.42f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a   = juce::jmap ((float)i, 0.f, 10.f, geom.startAngle, geom.endAngle);
        const float sinA = std::sin (a), cosA = std::cos (a);
        g.drawLine ({ c.x + sinA * r * 0.80f, c.y - cosA * r * 0.80f,
                      c.x + sinA * r * 0.92f, c.y - cosA * r * 0.92f },
                    (i % 5 == 0) ? 2.f : 1.f);
    }

    // Pointer with chromatic aberration
    const auto tip = geom.pointerTip (0.75f);
    g.setColour (Colours::vhsCyan.withAlpha (0.78f));
    g.drawLine ({ c, tip }, 3.0f);
    g.setColour (Colours::vhsMagenta.withAlpha (0.26f));
    g.drawLine ({ c.x + 0.5f, c.y, tip.x + 0.5f, tip.y }, 1.5f);
}

// ===========================================================================
// Pro knob (LookAndFeel-driven, for sliders not using VHSKnobPro directly)
// ===========================================================================
void VHSLookAndFeel::drawProKnob (juce::Graphics& g,
                                   const KnobGeometry& geom,
                                   juce::Slider& slider) const
{
    const float r  = geom.radius;
    const auto& c  = geom.centre;
    const auto  rt = 3.0f;

    // Glow
    Colours::drawGlow (g, geom.bodyRect, Colours::vhsMagenta, 0.20f, 1);

    // Radial body
    juce::ColourGradient radial (Colours::vhsDark.brighter (0.18f), c.x, c.y,
                                  Colours::vhsDark.darker   (0.58f), c.x, c.y + r,
                                  true);
    g.setGradientFill (radial);
    g.fillEllipse (geom.bodyRect);

    // Inner shadow
    juce::ColourGradient shadow (juce::Colours::black.withAlpha (0.38f),
                                  c.x - r * 0.4f, c.y - r * 0.4f,
                                  juce::Colours::transparentBlack,
                                  c.x + r * 0.4f, c.y + r * 0.4f, true);
    g.setGradientFill (shadow);
    g.fillEllipse (geom.bodyRect);

    // Rim
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.drawEllipse (geom.bodyRect, 1.2f);

    // Progress arc
    if (!geom.progressArc.isEmpty())
    {
        juce::ColourGradient arcGrad (Colours::vhsCyan,
                                       c.x, geom.bodyRect.getY(),
                                       Colours::vhsMagenta,
                                       c.x, geom.bodyRect.getBottom(), false);
        g.setGradientFill (arcGrad);
        g.strokePath (geom.progressArc,
                      juce::PathStrokeType (rt, juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
    }

    // Pointer: slim recessed indicator instead of a large triangular needle.
    const auto inner = geom.pointerTip (0.18f);
    const auto outer = geom.pointerTip (0.64f);
    const juce::Line<float> shadowLine (inner.translated (1.2f, 1.4f),
                                        outer.translated (1.2f, 1.4f));
    const juce::Line<float> recessLine (inner, outer);

    g.setColour (juce::Colours::black.withAlpha (0.70f));
    g.drawLine (shadowLine, 4.6f);
    g.setColour (juce::Colours::black.withAlpha (0.58f));
    g.drawLine (recessLine, 3.8f);
    g.setColour (Colours::vhsMagenta.withAlpha (0.78f));
    g.drawLine (recessLine, 2.0f);

    const auto tip = geom.pointerTip (0.67f);
    g.setColour (Colours::vhsCyan.withAlpha (0.55f));
    g.fillEllipse (tip.x - 2.0f, tip.y - 2.0f, 4.0f, 4.0f);

    juce::ignoreUnused (slider);
}

// ===========================================================================
// NeonRing knob (LookAndFeel-driven)
// ===========================================================================
void VHSLookAndFeel::drawNeonRingKnob (juce::Graphics& g,
                                        const KnobGeometry& geom,
                                        juce::Slider& slider) const
{
    const float r  = geom.radius;
    const auto& c  = geom.centre;
    const float rt = 5.0f;

    // Dark body
    juce::ColourGradient grad (Colours::vhsDark.brighter (0.10f), c.x, c.y - r * 0.35f,
                                Colours::vhsDark.darker   (0.62f), c.x, c.y + r, true);
    g.setGradientFill (grad);
    g.fillEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f);

    g.setColour (Colours::vhsGrid.withAlpha (0.34f));
    g.drawEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f, 0.8f);

    // Full-sweep dim ring
    g.setColour (Colours::vhsGrid.withAlpha (0.22f));
    g.strokePath (geom.sweepArc,
                  juce::PathStrokeType (rt, juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    // Dual-glow progress arc
    if (!geom.progressArc.isEmpty())
    {
        g.setColour (Colours::vhsBlue.withAlpha (0.14f));
        g.strokePath (geom.progressArc,
                      juce::PathStrokeType (rt * 2.2f, juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
        g.setColour (Colours::vhsCyan.withAlpha (0.32f));
        g.strokePath (geom.progressArc,
                      juce::PathStrokeType (rt * 1.2f, juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
        g.setColour (Colours::vhsCyan.withAlpha (0.82f));
        g.strokePath (geom.progressArc,
                      juce::PathStrokeType (rt * 0.42f, juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
    }

    // Chromatic pointer
    const auto tip = geom.pointerTip (0.68f);
    const juce::PathStrokeType ps (2.2f, juce::PathStrokeType::curved,
                                   juce::PathStrokeType::rounded);

    juce::Path pm;
    pm.startNewSubPath (c.x + 1.5f, c.y); pm.lineTo (tip.x + 1.5f, tip.y);
    g.setColour (Colours::vhsMagenta.withAlpha (0.32f));
    g.strokePath (pm, ps);

    juce::Path pn;
    pn.startNewSubPath (c.x, c.y); pn.lineTo (tip.x, tip.y);
    g.setColour (Colours::vhsCyan.withAlpha (0.82f));
    g.strokePath (pn, ps);

    g.setColour (juce::Colours::white.withAlpha (0.58f));
    g.fillEllipse (tip.x - 2.f, tip.y - 2.f, 4.f, 4.f);

    juce::ignoreUnused (slider);
}

// ===========================================================================
// Digital knob (LookAndFeel-driven, stateless — no flash support)
// ===========================================================================
void VHSLookAndFeel::drawDigitalKnob (juce::Graphics& g,
                                       const KnobGeometry& geom,
                                       float sliderPos, int stepCount,
                                       juce::Slider& slider) const
{
    const auto& c  = geom.centre;
    const float rt = 4.0f;

    const int lit = static_cast<int> (sliderPos * stepCount);

    juce::Array<juce::Path> segs;
    geom.buildSegmentPaths (segs, stepCount);

    const juce::PathStrokeType stroke (rt, juce::PathStrokeType::curved,
                                       juce::PathStrokeType::rounded);

    for (int i = 0; i < segs.size(); ++i)
    {
        if (i < lit)
        {
            g.setColour (Colours::vhsCyan.withAlpha (0.12f));
            g.strokePath (segs[i], juce::PathStrokeType (rt * 1.65f,
                                    juce::PathStrokeType::curved,
                                    juce::PathStrokeType::rounded));
            g.setColour (Colours::vhsCyan.withAlpha (0.74f));
            g.strokePath (segs[i], stroke);
        }
        else if (i == lit)
        {
            g.setColour (Colours::vhsMagenta.withAlpha (0.86f));
            g.strokePath (segs[i], stroke);
        }
        else
        {
            g.setColour (Colours::vhsGrid.withAlpha (0.16f));
            g.strokePath (segs[i], stroke);
        }
    }

    // Centre label
    const float br = geom.radius * 0.52f;
    g.setColour (Colours::vhsDark.withAlpha (0.92f));
    g.fillEllipse (c.x - br, c.y - br, br * 2.f, br * 2.f);

    const float fontSize = geom.radius * 0.36f;
    juce::GlyphArrangement ga;
    ga.addFittedText (Fonts::body (fontSize),
                      juce::String (slider.getValue(), 1),
                      c.x - geom.radius * 0.5f,
                      c.y - fontSize * 0.55f,
                      geom.radius, fontSize * 1.1f,
                      juce::Justification::centred, 1);
    g.setColour (Colours::vhsCyan.withAlpha (0.84f));
    ga.draw (g);
}

// ===========================================================================
// drawLinearSlider (unchanged)
// ===========================================================================
void VHSLookAndFeel::drawLinearSlider (juce::Graphics& g,
                                        int x, int y, int w, int h,
                                        float pos, float minPos, float maxPos,
                                        juce::Slider::SliderStyle style,
                                        juce::Slider& s)
{
    juce::ignoreUnused (minPos, maxPos, s);

    auto bounds = juce::Rectangle<float> ((float)x, (float)y,
                                           (float)w, (float)h).reduced (4.f);

    juce::Rectangle<float> track;

    if (style == juce::Slider::LinearHorizontal)
    {
        const float inset = bounds.getHeight() * 0.4f;
        track = { bounds.getX(), bounds.getY() + inset, bounds.getWidth(),
                  juce::jmax (1.f, bounds.getHeight() - 2.f * inset) };
    }
    else
    {
        const float inset = bounds.getWidth() * 0.4f;
        track = { bounds.getX() + inset, bounds.getY(),
                  juce::jmax (1.f, bounds.getWidth() - 2.f * inset), bounds.getHeight() };
    }

    g.setColour (Colours::vhsDark.darker (0.2f));
    g.fillRect (track);

    juce::ColourGradient gr (Colours::vhsPurple, track.getTopLeft(),
                              Colours::vhsBlue, track.getBottomRight(), false);
    g.setGradientFill (gr);

    g.fillRect ((style == juce::Slider::LinearHorizontal)
        ? juce::Rectangle<float> (track.getX(), track.getY(),
                                   pos - track.getX(), track.getHeight())
        : juce::Rectangle<float> (track.getX(), pos,
                                   track.getWidth(), track.getBottom() - pos));

    // Handle
    g.setColour (juce::Colours::white);
    auto handle = (style == juce::Slider::LinearHorizontal)
        ? juce::Rectangle<float> (pos - 5.f, bounds.getY(), 10.f, bounds.getHeight())
        : juce::Rectangle<float> (bounds.getX(), pos - 5.f, bounds.getWidth(), 10.f);

    g.fillRect (handle);
    g.setColour (Colours::vhsMagenta);
    g.drawRect (handle, 1.f);
}

// ===========================================================================
// drawButtonBackground (unchanged)
// ===========================================================================
void VHSLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                            juce::Button& b,
                                            const juce::Colour& bg,
                                            bool hover, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (2.f);
    auto c = down ? bg.darker (0.4f) : hover ? bg.brighter (0.2f) : bg;

    g.setColour (c);
    g.fillRoundedRectangle (r, 6.f);

    Colours::drawGlow (g, r.withSizeKeepingCentre (r.getWidth() * 0.7f,
                                                    r.getHeight() * 0.7f),
                       Colours::vhsBlue, hover ? 0.8f : 0.4f, 2);
}

} // namespace bvs
