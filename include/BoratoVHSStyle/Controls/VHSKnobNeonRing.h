#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BoratoVHSStyle/Controls/KnobStyle.h"
#include "BoratoVHSStyle/Controls/KnobGeometry.h"

namespace bvs {

/**
 * @brief Minimalist dark-body knob with a prominent dual-glow neon progress
 *        arc, chromatic-aberration pointer, and optional audio-reactive pulse.
 *
 * @code
 *   bvs::VHSKnobNeonRing knob;
 *   knob.setRingThickness (6.f);
 *   // Drive from audio thread via callAsync:
 *   knob.setPulseIntensity (rmsLevel);
 *   addAndMakeVisible (knob);
 * @endcode
 */
class VHSKnobNeonRing : public juce::Slider
{
public:
    explicit VHSKnobNeonRing (KnobStyle style = KnobStyle::NeonRing);
    ~VHSKnobNeonRing() override = default;

    // ---- Visual configuration --------------------------------------------

    /** Outer neon ring stroke width [2, 16]. Default 5. */
    void setRingThickness (float px) noexcept;

    /**
     * Drive the pulsing halo from an external level [0, 1].
     * Safe to call from the message thread only.
     * Use juce::MessageManager::callAsync() if feeding from the audio thread.
     */
    void setPulseIntensity (float level) noexcept;

    // ---- juce::Component --------------------------------------------------
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    KnobStyle style_          { KnobStyle::NeonRing };
    float     ringThickness_  { 5.0f };
    float     pulseIntensity_ { 0.0f };

    KnobGeometry geom_;

    void drawBackground     (juce::Graphics&) const;
    void drawSweepRing      (juce::Graphics&) const;
    void drawDualGlowArc    (juce::Graphics&) const;
    void drawChromaticPointer (juce::Graphics&) const;
    void drawPulseHalo      (juce::Graphics&) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VHSKnobNeonRing)
};

} // namespace bvs
