#include "BoratoVHSStyle/Backgrounds/ScanlinesOverlay.h"

namespace bvs {

ScanlinesOverlay::ScanlinesOverlay() { startTimerHz(60); }

void ScanlinesOverlay::setOpacity(float o) { opacity = juce::jlimit(0.f, 1.f, o); }
void ScanlinesOverlay::setSpeed(float p)   { speed = p; }

void ScanlinesOverlay::timerCallback() { offset = std::fmod(offset + speed, 3.f); repaint(); }

void ScanlinesOverlay::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    g.setColour(juce::Colours::black.withAlpha(opacity));

    for (float y = offset; y < b.getBottom(); y += 3.0f)
        g.fillRect(b.getX(), y, b.getWidth(), 1.0f);
}

}
