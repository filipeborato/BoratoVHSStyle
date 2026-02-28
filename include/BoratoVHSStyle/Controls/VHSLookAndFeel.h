#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BoratoVHSStyle/Controls/KnobStyle.h"
#include "BoratoVHSStyle/Controls/KnobGeometry.h"

namespace bvs {

/**
 * @brief Master JUCE LookAndFeel for all BoratoVHSStyle components.
 *
 * drawRotarySlider() dispatches to per-style helpers based on the
 * "bvs_knobStyle" property stored in the juce::Slider's properties map.
 * Sliders without the property fall through to the Basic (original) renderer.
 *
 * JUCE 8 note: getDefaultMetricsKind() returns legacy to preserve cross-
 * platform text sizing for the VHSKnobDigital centre label.
 */
class VHSLookAndFeel : public juce::LookAndFeel_V4
{
public:
    VHSLookAndFeel();

    // ---- JUCE 8 – stable text metrics across platforms -------------------
    juce::TypefaceMetricsKind getDefaultMetricsKind() const override
    {
        return juce::TypefaceMetricsKind::legacy;
    }

    // ---- LookAndFeel overrides -------------------------------------------
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPosProportional,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&,
                               const juce::Colour& bg,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    juce::Typeface::Ptr getTypefaceForFont (const juce::Font&) override;

private:
    // ---- Per-style rotary draw helpers (geometry already computed) -------
    void drawBasicKnob    (juce::Graphics&, const KnobGeometry&, juce::Slider&) const;
    void drawProKnob      (juce::Graphics&, const KnobGeometry&, juce::Slider&) const;
    void drawNeonRingKnob (juce::Graphics&, const KnobGeometry&, juce::Slider&) const;
    void drawDigitalKnob  (juce::Graphics&, const KnobGeometry&,
                           float sliderPos, int stepCount, juce::Slider&) const;

    // Reusable geometry instance — rebuilt per drawRotarySlider call so it
    // is compatible with multiple independent sliders using this LookAndFeel.
    mutable KnobGeometry lafGeom_;
};

} // namespace bvs

