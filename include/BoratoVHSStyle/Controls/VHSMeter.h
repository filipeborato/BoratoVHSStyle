#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSMeter : public juce::Component, private juce::Timer
{
public:
    VHSMeter();

    void setLevel(float linear); // 0..1

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    float current = 0.f, peak = 0.f, hold = 0.f; // afterglow
};

}
