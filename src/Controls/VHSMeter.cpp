#include "BoratoVHSStyle/Controls/VHSMeter.h"

#include "BoratoVHSStyle/Colours.h"

namespace bvs {

VHSMeter::VHSMeter(){ startTimerHz(60); }

void VHSMeter::setLevel(float v){ current = juce::jlimit(0.f,1.f,v); }

void VHSMeter::timerCallback(){

    peak = juce::jmax(peak * 0.90f, current);

    hold = juce::jmax(hold * 0.97f, current); // rastro longo

    repaint();

}

void VHSMeter::paint(juce::Graphics& g)

{

    auto r = getLocalBounds().toFloat();

    g.setColour(Colours::vhsDark); g.fillRoundedRectangle(r, 3.f);

    auto h = r.getHeight() * current;

    juce::Rectangle<float> bar = { r.getX()+4, r.getBottom()-h-4, r.getWidth()-8, h };

    juce::ColourGradient gr(Colours::vhsBlue, bar.getBottomLeft(), Colours::vhsMagenta, bar.getTopRight(), false);

    g.setGradientFill(gr); g.fillRect(bar);

    // afterglow (hold)

    auto hh = r.getHeight() * hold;

    g.setColour(Colours::vhsCyan.withAlpha(0.35f));

    g.fillRect(juce::Rectangle<float>(bar).withY(r.getBottom()-hh-4).withHeight(3.f));

}

}
