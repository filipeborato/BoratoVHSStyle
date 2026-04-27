#include "BoratoVHSStyle/Controls/VHSKnobPro.h"

#include "BoratoVHSStyle/Colours.h"
#include "BoratoVHSStyle/Fonts.h"

#include <cmath>
#include <random>

namespace bvs {

// ---------------------------------------------------------------------------
VHSKnobPro::VHSKnobPro (KnobStyle style)
    : style_ (style),
      accentColour_ (Colours::vhsMagenta)
{
    setSliderStyle  (juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

    // Expose the style tag so VHSLookAndFeel can dispatch if applied
    getProperties().set ("bvs_knobStyle", static_cast<int> (style_));
}

// ---- Configuration --------------------------------------------------------
void VHSKnobPro::setGlowStrength  (float g)        noexcept { glowStrength_  = juce::jlimit (0.f, 1.f, g);       repaint(); }
void VHSKnobPro::setMarkerCount   (int count)       noexcept { markerCount_   = juce::jlimit (2, 24, count);  ticksDirty_ = true; repaint(); }
void VHSKnobPro::setAccentColour  (juce::Colour c)  noexcept { accentColour_  = c;                                repaint(); }
void VHSKnobPro::setRingThickness (float px)        noexcept { ringThickness_ = juce::jlimit (1.f, 12.f, px); ticksDirty_ = true; repaint(); }

// ---- Resize ---------------------------------------------------------------
void VHSKnobPro::resized()
{
    textureDirty_ = true;
    ticksDirty_   = true;
}

// ---- Texture (micro procedural noise) ------------------------------------
void VHSKnobPro::rebuildTexture (int w, int h)
{
    microTexture_ = juce::Image (juce::Image::ARGB, juce::jmax (1, w), juce::jmax (1, h), true);

    // Deterministic seed — same look every launch
    std::mt19937 rng (0xC0FFEE42u);
    std::uniform_int_distribution<int>    brightDist (texBrightMin_, texBrightMax_);
    std::uniform_real_distribution<float> alphaDist  (0.0f, texAlphaMax_);

    juce::Image::BitmapData bm (microTexture_, juce::Image::BitmapData::writeOnly);

    for (int y = 0; y < bm.height; ++y)
        for (int x = 0; x < bm.width; ++x)
        {
            const uint8_t v = static_cast<uint8_t> (brightDist (rng));
            const uint8_t a = static_cast<uint8_t> (alphaDist (rng) * 255.f);
            bm.setPixelColour (x, y, juce::Colour (v, v, v, a));
        }

    textureDirty_ = false;
}

// ---- paint() --------------------------------------------------------------
void VHSKnobPro::paint (juce::Graphics& g)
{
    const auto bounds  = getLocalBounds().toFloat();
    const float pos    = static_cast<float> (valueToProportionOfLength (getValue()));

    // Fixed JUCE rotary angle convention (bottom-left start, clockwise)
    const float rotaryStart = juce::MathConstants<float>::pi * 1.25f;
    const float rotaryEnd   = juce::MathConstants<float>::pi * 2.75f;

    if (geom_.isDirty (bounds, pos, ringThickness_))
        geom_.rebuild (bounds, pos, rotaryStart, rotaryEnd, ringThickness_);

    // --- micro texture (rebuild only on resize) --------------------------
    if (textureDirty_)
        rebuildTexture (getWidth(), getHeight());

    // --- tick marks (rebuild only when count/bounds change) ---------------
    if (ticksDirty_)
    {
        geom_.buildTickPaths (cachedTickPaths_, markerCount_);
        ticksDirty_ = false;
    }

    drawGlowRing    (g);
    drawBody        (g);
    drawProgressArc (g);
    drawTicks       (g);
    drawPointer     (g);
}

// ---- Glow ring -----------------------------------------------------------
void VHSKnobPro::drawGlowRing (juce::Graphics& g) const
{
    if (glowStrength_ < 0.01f)
        return;

    Colours::drawGlow (g,
                       geom_.bodyRect,
                       accentColour_,
                       glowStrength_ * 0.28f,
                       1);
}

// ---- Body with radial gradient + inner shadow + micro texture ------------
void VHSKnobPro::drawBody (juce::Graphics& g) const
{
    const float r = geom_.radius;
    const auto& c = geom_.centre;

    const auto outer = geom_.bodyRect.expanded (ringThickness_ * 0.42f);

    juce::ColourGradient rimGrad (
        Colours::vhsGrid.brighter (0.28f), outer.getCentreX(), outer.getY(),
        juce::Colours::black.withAlpha (0.92f), outer.getCentreX(), outer.getBottom(),
        false);

    g.setGradientFill (rimGrad);
    g.fillEllipse (outer);

    g.setColour (juce::Colours::black.withAlpha (0.72f));
    g.drawEllipse (outer, 1.4f);

    g.setColour (accentColour_.withAlpha (0.16f));
    g.drawEllipse (outer.reduced (1.6f), 0.8f);

    // --- radial gradient (dark edge → bright centre) ----------------------
    juce::ColourGradient radial (
        Colours::vhsDark.brighter (0.18f),   // centre
        c.x, c.y,
        Colours::vhsDark.darker  (0.58f),   // edge
        c.x, c.y + r,
        true);                               // radial

    g.setGradientFill (radial);
    g.fillEllipse (geom_.bodyRect);

    // --- simulated inner shadow (dark top-left arc) ------------------------
    juce::ColourGradient innerShadow (
        juce::Colours::black.withAlpha (0.38f),
        c.x - r * 0.4f, c.y - r * 0.4f,
        juce::Colours::transparentBlack,
        c.x + r * 0.4f, c.y + r * 0.4f,
        true);

    g.setGradientFill (innerShadow);
    g.fillEllipse (geom_.bodyRect);

    // --- outer rim (thin stroke) ------------------------------------------
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.drawEllipse (geom_.bodyRect, 1.2f);

    // --- micro texture overlay --------------------------------------------
    if (microTexture_.isValid())
    {
        juce::Graphics::ScopedSaveState ss (g);
        juce::Path clipPath;
        clipPath.addEllipse (geom_.bodyRect);
        g.reduceClipRegion (clipPath, juce::AffineTransform{});
        g.setOpacity (0.26f);
        g.drawImage (microTexture_, geom_.bodyRect,
                     juce::RectanglePlacement::stretchToFit);
    }
}

// ---- Progress arc --------------------------------------------------------
void VHSKnobPro::drawProgressArc (juce::Graphics& g) const
{
    if (geom_.progressArc.isEmpty())
        return;

    // Dual pass: outer glow then bright core
    juce::PathStrokeType stroke (ringThickness_, juce::PathStrokeType::curved,
                                 juce::PathStrokeType::rounded);

    // Outer glow pass (wider, more transparent)
    g.setColour (accentColour_.withAlpha (0.13f));
    juce::PathStrokeType glowStroke (ringThickness_ * 2.4f, juce::PathStrokeType::curved,
                                     juce::PathStrokeType::rounded);
    g.strokePath (geom_.progressArc, glowStroke);

    // Core arc with gradient: cyan → accent
    juce::ColourGradient arcGrad (Colours::vhsCyan.withAlpha (0.78f),
                                  geom_.centre.x, geom_.bodyRect.getY(),
                                  accentColour_.withAlpha (0.92f),
                                  geom_.centre.x, geom_.bodyRect.getBottom(),
                                  false);
    g.setGradientFill (arcGrad);
    g.strokePath (geom_.progressArc, stroke);
}

// ---- Tick marks ----------------------------------------------------------
void VHSKnobPro::drawTicks (juce::Graphics& g)
{
    if (cachedTickPaths_.isEmpty())
        return;

    // We draw the path with varying widths by iterating manually
    // Actually, buildTickPaths bakes all ticks into one Path for speed.
    // We draw the whole path once at base alpha, then re-draw major ticks
    // with double thickness by building a separate major-tick path.

    g.setColour (Colours::vhsGrid.brighter (0.65f).withAlpha (0.42f));
    g.strokePath (cachedTickPaths_,
                  juce::PathStrokeType (1.0f, juce::PathStrokeType::mitered,
                                        juce::PathStrokeType::butt));

    // Major ticks (every 5th)
    juce::Path majorTicks;
    const int total = (markerCount_ - 1);
    const float rotaryStart = juce::MathConstants<float>::pi * 1.25f;
    const float rotaryEnd   = juce::MathConstants<float>::pi * 2.75f;

    for (int i = 0; i <= total; i += 5)
    {
        const float t = (total > 0) ? static_cast<float> (i) / total : 0.f;
        const float a = rotaryStart + t * (rotaryEnd - rotaryStart);
        const float sinA = std::sin (a), cosA = std::cos (a);
        const auto& c = geom_.centre;
        const float r = geom_.radius;

        majorTicks.startNewSubPath (c.x + sinA * r * 0.75f, c.y - cosA * r * 0.75f);
        majorTicks.lineTo          (c.x + sinA * r * 0.92f, c.y - cosA * r * 0.92f);
    }

    g.setColour (Colours::vhsCyan.withAlpha (0.62f));
    g.strokePath (majorTicks,
                  juce::PathStrokeType (2.0f, juce::PathStrokeType::mitered,
                                        juce::PathStrokeType::butt));
}

// ---- Triangular pointer with shadow  ------------------------------------
void VHSKnobPro::drawPointer (juce::Graphics& g) const
{
    const auto inner = geom_.pointerTip (0.18f);
    const auto outer = geom_.pointerTip (0.64f);

    const juce::Line<float> shadowLine (inner.translated (1.2f, 1.4f),
                                        outer.translated (1.2f, 1.4f));
    const juce::Line<float> recessLine (inner, outer);

    g.setColour (juce::Colours::black.withAlpha (0.70f));
    g.drawLine (shadowLine, 4.6f);

    g.setColour (juce::Colours::black.withAlpha (0.58f));
    g.drawLine (recessLine, 3.8f);

    g.setColour (accentColour_.withAlpha (0.78f));
    g.drawLine (recessLine, 2.0f);

    const auto tip = geom_.pointerTip (0.67f);
    g.setColour (Colours::vhsCyan.withAlpha (0.55f));
    g.fillEllipse (tip.x - 2.0f, tip.y - 2.0f, 4.0f, 4.0f);
}

// ---- Optional specular pass (unused for Pro knob) ------------------------
void VHSKnobPro::drawSpecular (juce::Graphics& g) const
{
    // Intentionally left blank: the Pro knob no longer renders a specular
    // highlight, but this method is kept as a compatibility hook.
    juce::ignoreUnused (g);
}

} // namespace bvs
