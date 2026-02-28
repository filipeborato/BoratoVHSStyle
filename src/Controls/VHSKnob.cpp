#include "BoratoVHSStyle/Controls/VHSKnob.h"

namespace bvs {

VHSKnob::VHSKnob()

{

    setSliderStyle(RotaryHorizontalVerticalDrag);

    setTextBoxStyle(NoTextBox, false, 0,0);

    startTimerHz(60);

}

void VHSKnob::setIdleWobble(bool e){ wobble = e; }

void VHSKnob::setGlowStrength(float s){ glow = juce::jlimit(0.f,1.f,s); }

void VHSKnob::timerCallback()

{

    if (! wobble || isMouseOverOrDragging()) return;

    phase += 0.04f;

    auto offset = 0.0018f * std::sin(phase); // "respirar" sem incomodar automação

    setValue(juce::jlimit(getMinimum(), getMaximum(), getValue() + offset), juce::dontSendNotification);

}

}
