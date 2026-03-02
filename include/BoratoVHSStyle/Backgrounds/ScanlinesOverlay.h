#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class ScanlinesOverlay : public juce::Component, private juce::Timer
{
public:
    ScanlinesOverlay();

    void setOpacity(float o); // 0..1
    void setSpeed(float pixelsPerTick);

    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;

    float offset = 0.f, speed = 0.6f, opacity = 0.20f;
};

}
