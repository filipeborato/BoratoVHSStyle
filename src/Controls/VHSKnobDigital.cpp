#include "BoratoVHSStyle/Controls/VHSKnobDigital.h"

#include "BoratoVHSStyle/Colours.h"
#include "BoratoVHSStyle/Fonts.h"

namespace bvs {

// ---------------------------------------------------------------------------
VHSKnobDigital::VHSKnobDigital (KnobStyle style)
    : style_ (style)
{
    setSliderStyle  (juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    getProperties().set ("bvs_knobStyle", static_cast<int> (style_));

    // Default formatter: 1 decimal place
    valueFmt_ = [] (double v) { return juce::String (v, 1); };

    startTimerHz (60);
}

VHSKnobDigital::~VHSKnobDigital()
{
    stopTimer();
}

// ---- Config ---------------------------------------------------------------
void VHSKnobDigital::setStepCount (int steps) noexcept
{
    stepCount_  = juce::jlimit (2, 64, steps);
    segsDirty_  = true;
    repaint();
}

void VHSKnobDigital::setValueFormat (std::function<juce::String (double)> fmt)
{
    valueFmt_     = std::move (fmt);
    lastLabelStr_ = {};   // force rebuild
    repaint();
}

// ---- Resize ---------------------------------------------------------------
void VHSKnobDigital::resized()
{
    segsDirty_ = true;
}

// ---- Value change (slider callback) ---------------------------------------
void VHSKnobDigital::valueChanged()
{
    const double v        = getValue();
    const double range    = getMaximum() - getMinimum();
    const int    newStep  = (range > 0.0)
                            ? static_cast<int> ((v - getMinimum()) / range * stepCount_)
                            : 0;

    if (lastStepIndex_ >= 0 && newStep != lastStepIndex_)
        flashFrames_ = 5;   // flash for 5 frames @ 60fps ≈ 83ms

    lastStepIndex_ = newStep;
    repaint();
}

// ---- Timer (flash decay) --------------------------------------------------
void VHSKnobDigital::timerCallback()
{
    if (flashFrames_ > 0)
    {
        --flashFrames_;
        repaint();
    }
}

// ---- Label cache ----------------------------------------------------------
void VHSKnobDigital::rebuildLabel()
{
    const juce::String str = valueFmt_ (getValue());
    if (str == lastLabelStr_) return;

    lastLabelStr_ = str;
    labelGlyphs_.clear();

    const float fontSize = geom_.radius * 0.38f;
    const juce::Font font = Fonts::body (fontSize);

    labelGlyphs_.addFittedText (font, str,
                                 geom_.centre.x - geom_.radius * 0.5f,
                                 geom_.centre.y - fontSize * 0.55f,
                                 geom_.radius,
                                 fontSize * 1.1f,
                                 juce::Justification::centred, 1);
}

// ---- paint() --------------------------------------------------------------
void VHSKnobDigital::paint (juce::Graphics& g)
{
    const auto  bounds      = getLocalBounds().toFloat();
    const float pos         = static_cast<float> (valueToProportionOfLength (getValue()));
    const float rotaryStart = juce::MathConstants<float>::pi * 1.25f;
    const float rotaryEnd   = juce::MathConstants<float>::pi * 2.75f;

    if (geom_.isDirty (bounds, pos, ringThickness_))
    {
        geom_.rebuild (bounds, pos, rotaryStart, rotaryEnd, ringThickness_);
        segsDirty_ = true;
    }

    if (segsDirty_)
    {
        geom_.buildSegmentPaths (segPaths_, stepCount_);
        segsDirty_ = false;
    }

    rebuildLabel();

    drawOuterArc    (g);
    drawSegments    (g, pos);
    drawFlashRing   (g);
    drawCentreLabel (g);
}

// ---- Full-range outer arc (dim) ------------------------------------------
void VHSKnobDigital::drawOuterArc (juce::Graphics& g) const
{
    const float outerRadius = geom_.radius + ringThickness_ * 1.8f;
    const auto& c           = geom_.centre;

    juce::Path outerArc;
    outerArc.addCentredArc (c.x, c.y,
                             outerRadius, outerRadius,
                             0.f,
                             geom_.startAngle, geom_.endAngle,
                             true);

    g.setColour (Colours::vhsGrid.withAlpha (0.18f));
    g.strokePath (outerArc,
                  juce::PathStrokeType (1.0f, juce::PathStrokeType::curved,
                                        juce::PathStrokeType::butt));
}

// ---- Segmented ring ------------------------------------------------------
void VHSKnobDigital::drawSegments (juce::Graphics& g, float pos) const
{
    const int   lit = static_cast<int> (pos * stepCount_);

    const juce::PathStrokeType stroke (ringThickness_,
                                       juce::PathStrokeType::curved,
                                       juce::PathStrokeType::rounded);

    for (int i = 0; i < segPaths_.size(); ++i)
    {
        if (i < lit)
        {
            // Lit segment: glow pass + bright core
            g.setColour (Colours::vhsCyan.withAlpha (0.28f));
            g.strokePath (segPaths_[i],
                          juce::PathStrokeType (ringThickness_ * 2.2f,
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

            g.setColour (Colours::vhsCyan.withAlpha (0.92f));
            g.strokePath (segPaths_[i], stroke);
        }
        else if (i == lit)
        {
            // Current (leading edge) segment — accent colour
            g.setColour (Colours::vhsMagenta.withAlpha (0.35f));
            g.strokePath (segPaths_[i],
                          juce::PathStrokeType (ringThickness_ * 2.0f,
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

            g.setColour (Colours::vhsMagenta);
            g.strokePath (segPaths_[i], stroke);
        }
        else
        {
            // Unlit segment
            g.setColour (Colours::vhsGrid.withAlpha (0.22f));
            g.strokePath (segPaths_[i], stroke);
        }
    }
}

// ---- Step-crossing flash ring --------------------------------------------
void VHSKnobDigital::drawFlashRing (juce::Graphics& g) const
{
    if (flashFrames_ <= 0)
        return;

    const float alpha = static_cast<float> (flashFrames_) / 5.0f * 0.45f;
    Colours::drawGlow (g, geom_.bodyRect, Colours::vhsMagenta, alpha, 1);
}

// ---- Centre value label --------------------------------------------------
void VHSKnobDigital::drawCentreLabel (juce::Graphics& g) const
{
    // Fill circular dark background for legibility
    const float br = geom_.radius * 0.52f;
    const auto& c  = geom_.centre;

    g.setColour (Colours::vhsDark.withAlpha (0.88f));
    g.fillEllipse (c.x - br, c.y - br, br * 2.f, br * 2.f);

    g.setColour (Colours::vhsCyan);
    labelGlyphs_.draw (g);
}

} // namespace bvs
