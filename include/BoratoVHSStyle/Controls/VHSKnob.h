#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSKnob : public juce::Slider, private juce::Timer
{
public:
    VHSKnob();

    void setIdleWobble(bool enabled);
    void setGlowStrength(float s); // 0..1

private:
    void timerCallback() override;

    bool  wobble = true;
    float phase  = 0.f;
    float glow   = 0.6f;
};

}
