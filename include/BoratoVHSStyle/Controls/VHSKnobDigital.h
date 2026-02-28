#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BoratoVHSStyle/Controls/KnobStyle.h"
#include "BoratoVHSStyle/Controls/KnobGeometry.h"
#include "BoratoVHSStyle/Fonts.h"

#include <functional>

namespace bvs {

/**
 * @brief Encoder-style digital knob with segmented ring, step-crossing flash,
 *        and centre value label in monospace font.
 *
 * @code
 *   bvs::VHSKnobDigital knob;
 *   knob.setStepCount (16);
 *   knob.setValueFormat ([] (double v) { return juce::String (v, 1) + " dB"; });
 *   addAndMakeVisible (knob);
 * @endcode
 */
class VHSKnobDigital : public juce::Slider,
                        private juce::Timer
{
public:
    explicit VHSKnobDigital (KnobStyle style = KnobStyle::Digital);
    ~VHSKnobDigital() override;

    // ---- Visual configuration --------------------------------------------

    /** Number of illuminated segments [2, 64]. Default 16. */
    void setStepCount (int steps) noexcept;

    /**
     * Custom value formatter for the centre label.
     * Called only when the value changes — result is cached.
     */
    void setValueFormat (std::function<juce::String (double)> fmt);

    // ---- juce::Component / juce::Slider ----------------------------------
    void paint  (juce::Graphics&) override;
    void resized() override;

private:
    // Slider observer to detect step-threshold crossings
    void valueChanged() override;

    // Timer drives flash decay (60 fps)
    void timerCallback() override;

    // ---- Config -----------------------------------------------------------
    KnobStyle   style_     { KnobStyle::Digital };
    int         stepCount_ { 16 };
    std::function<juce::String (double)> valueFmt_;

    // ---- Geometry cache ---------------------------------------------------
    KnobGeometry            geom_;
    juce::Array<juce::Path> segPaths_;
    bool                    segsDirty_ { true };

    // ---- Label cache ------------------------------------------------------
    juce::GlyphArrangement  labelGlyphs_;
    juce::String            lastLabelStr_;
    void                    rebuildLabel();

    // ---- Flash state ------------------------------------------------------
    int   flashFrames_    { 0 };       // >0 while a step-crossing flash is active
    int   lastStepIndex_  { -1 };      // previous step index for crossing detection
    float ringThickness_  { 4.0f };

    void drawOuterArc     (juce::Graphics&) const;
    void drawSegments     (juce::Graphics&, float pos) const;
    void drawFlashRing    (juce::Graphics&) const;
    void drawCentreLabel  (juce::Graphics&) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VHSKnobDigital)
};

} // namespace bvs
