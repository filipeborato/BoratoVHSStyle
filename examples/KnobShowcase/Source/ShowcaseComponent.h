#pragma once

#if __has_include(<JuceHeader.h>)
    #include <JuceHeader.h>                          // Projucer build
#else
    #include <juce_gui_basics/juce_gui_basics.h>     // CMake build
    #include <juce_gui_extra/juce_gui_extra.h>
    #include <BoratoVHSStyle/BoratoVHSStyle.h>
#endif

/**
 * ShowcaseComponent — demonstrates all 3 VHS knob variants side by side.
 *
 * Layout (left→right):
 *   [VHSKnobPro]  [VHSKnobNeonRing]  [VHSKnobDigital]
 *   [VHSMeter]        [VHSMeter]         [VHSMeter]
 *   Label             Label              Label
 *
 * A VHSGridBackground + ScanlinesOverlay + VHSNoiseLayer fills the window.
 */
class ShowcaseComponent final : public juce::Component,
                                 private juce::Timer
{
public:
    ShowcaseComponent();
    ~ShowcaseComponent() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    // ---- Background layers -----------------------------------------------
    bvs::VHSGridBackground  grid_;
    bvs::ScanlinesOverlay   scanlines_;
    bvs::VHSNoiseLayer      noise_;

    // ---- Look and feel ---------------------------------------------------
    bvs::VHSLookAndFeel     laf_;

    // ---- Knobs -----------------------------------------------------------
    bvs::VHSKnobPro        knobPro_;
    bvs::VHSKnobNeonRing   knobNeon_;
    bvs::VHSKnobDigital    knobDigital_;

    // ---- Meters (fed by knob values) ------------------------------------
    bvs::VHSMeter          meterPro_;
    bvs::VHSMeter          meterNeon_;
    bvs::VHSMeter          meterDigital_;

    // ---- Labels ---------------------------------------------------------
    juce::Label  labelPro_, labelNeon_, labelDigital_;

    // ---- Pulse slider (controls NeonRing halo) --------------------------
    juce::Slider pulseSlider_;
    juce::Label  pulseLabel_;

    // ---- Timer drives knob→meter animation at 60fps ---------------------
    void timerCallback() override;

    void setupKnob  (juce::Slider& k, double min, double max, double def);
    void setupLabel (juce::Label& l,  const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShowcaseComponent)
};
