#include "MainComponent.h"

#include <cmath>

MainComponent::MainComponent()
{
    setSize(720, 420);

    addAndMakeVisible(grid);
    addAndMakeVisible(noise);
    addAndMakeVisible(scanlines);
    addAndMakeVisible(knob1);
    addAndMakeVisible(knob2);
    addAndMakeVisible(sliderH);
    addAndMakeVisible(sliderV);
    addAndMakeVisible(toggle);
    addAndMakeVisible(button);
    addAndMakeVisible(meter);

    bvs::ThemeConfig theme;
    theme.saturation = 1.0f;
    theme.glowStrength = 0.9f;
    theme.neonBoost = 1.1f;

    bvs::applyThemeToTree(*this, lookAndFeel, theme);

    startTimerHz(60);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.setFont(bvs::Fonts::heading(22.0f));
    g.drawText("BoratoVHSStyle App", getLocalBounds().removeFromTop(28), juce::Justification::centred);

    g.setFont(bvs::Fonts::body(12.0f));
    g.drawFittedText("Executable harness for debugging the style library",
                     getLocalBounds().removeFromBottom(24),
                     juce::Justification::centred,
                     1);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    grid.setBounds(bounds);
    noise.setBounds(bounds);
    scanlines.setBounds(bounds);

    auto area = bounds.reduced(16);
    juce::ignoreUnused(area.removeFromTop(48));
    juce::ignoreUnused(area.removeFromBottom(60));

    auto left = area.removeFromLeft(area.getWidth() / 2);
    knob1.setBounds(left.removeFromTop(left.getHeight() / 2).reduced(12));
    knob2.setBounds(left.reduced(12));

    auto right = area;
    sliderV.setBounds(right.removeFromLeft(60).reduced(8));
    sliderH.setBounds(right.removeFromTop(60).reduced(8));

    auto buttonRow = right.removeFromTop(40);
    toggle.setBounds(buttonRow.removeFromLeft(120).reduced(6));
    button.setBounds(buttonRow.removeFromLeft(120).reduced(6));

    meter.setBounds(right.reduced(6));
}

void MainComponent::timerCallback()
{
    phase += 0.03f;

    auto level = 0.5f * (1.0f + std::sin(phase * 1.3f) * std::sin(phase * 0.37f));

    meter.setLevel(level);
    noise.setAmount(0.10f + 0.25f * level);
}
