#pragma once

#include <JuceHeader.h>
#include <BoratoVHSStyle/BoratoVHSStyle.h>

class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    bvs::VHSLookAndFeel lookAndFeel;
    bvs::VHSGridBackground grid;
    bvs::VHSNoiseLayer noise;
    bvs::ScanlinesOverlay scanlines;
    bvs::VHSKnob knob1;
    bvs::VHSKnob knob2;
    bvs::VHSSlider sliderH { true };
    bvs::VHSSlider sliderV { false };
    bvs::VHSToggle toggle { "GLITCH" };
    bvs::VHSMomentary button { "PUNCH" };
    bvs::VHSMeter meter;
    float phase = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
