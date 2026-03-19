#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSNoiseLayer : public juce::Component, private juce::Timer
{
public:
    VHSNoiseLayer();

    void setAmount(float a); // 0..1
    void setFPS(int fps);

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;
    void regenerate();

    float amount = 0.06f;  // 0.15f era muito alto para grain escuro
    juce::Image noise { juce::Image::ARGB, 16, 16, true };
};

}
