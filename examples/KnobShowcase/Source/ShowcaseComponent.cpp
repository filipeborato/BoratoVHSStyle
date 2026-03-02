#include "ShowcaseComponent.h"

#include <BoratoVHSStyle/BoratoVHSStyle.h>
#include <BoratoVHSStyle/Colours.h>
#include <BoratoVHSStyle/Fonts.h>

using namespace bvs;

// ---------------------------------------------------------------------------
ShowcaseComponent::ShowcaseComponent()
{
    setOpaque (true);
    setLookAndFeel (&laf_);

    // ---- Background init --------------------------------------------------
    grid_.setSpacing (24);
    scanlines_.setOpacity (0.12f);
    scanlines_.setSpeed (0.5f);
    noise_.setAmount (0.06f);
    noise_.setFPS (24);

    addAndMakeVisible (grid_);
    addAndMakeVisible (scanlines_);
    addAndMakeVisible (noise_);

    // ---- VHSKnobPro -------------------------------------------------------
    knobPro_.setRange (-48.0, 0.0, 0.1);
    knobPro_.setValue (-12.0, juce::dontSendNotification);
    knobPro_.setAccentColour (Colours::vhsMagenta);
    knobPro_.setMarkerCount (11);
    knobPro_.setGlowStrength (0.65f);
    knobPro_.setRingThickness (4.f);
    addAndMakeVisible (knobPro_);

    // ---- VHSKnobNeonRing --------------------------------------------------
    knobNeon_.setRange (0.0, 100.0, 0.1);
    knobNeon_.setValue (50.0, juce::dontSendNotification);
    knobNeon_.setRingThickness (6.f);
    addAndMakeVisible (knobNeon_);

    // ---- VHSKnobDigital ---------------------------------------------------
    knobDigital_.setRange (-24.0, 24.0, 1.0);
    knobDigital_.setValue (0.0, juce::dontSendNotification);
    knobDigital_.setStepCount (16);
    knobDigital_.setValueFormat ([] (double v)
    {
        return (v >= 0.0 ? "+" : "") + juce::String (v, 1) + " dB";
    });
    addAndMakeVisible (knobDigital_);

    // ---- Meters -----------------------------------------------------------
    meterPro_.setSize (24, 120);
    meterNeon_.setSize (24, 120);
    meterDigital_.setSize (24, 120);
    addAndMakeVisible (meterPro_);
    addAndMakeVisible (meterNeon_);
    addAndMakeVisible (meterDigital_);

    // ---- Pulse slider (drives NeonRing halo) ------------------------------
    pulseSlider_.setRange (0.0, 1.0);
    pulseSlider_.setValue (0.0);
    pulseSlider_.setSliderStyle (juce::Slider::LinearHorizontal);
    pulseSlider_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    pulseSlider_.onValueChange = [this]
    {
        knobNeon_.setPulseIntensity (static_cast<float> (pulseSlider_.getValue()));
    };
    addAndMakeVisible (pulseSlider_);

    // ---- Labels -----------------------------------------------------------
    setupLabel (labelPro_,     "VHSKnobPro");
    setupLabel (labelNeon_,    "VHSKnobNeonRing");
    setupLabel (labelDigital_, "VHSKnobDigital");
    setupLabel (pulseLabel_,   "Pulse Intensity");

    setSize (700, 510);
    startTimerHz (60);
}

ShowcaseComponent::~ShowcaseComponent()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

// ---------------------------------------------------------------------------
void ShowcaseComponent::setupKnob (juce::Slider& k, double mn, double mx, double def)
{
    k.setRange (mn, mx);
    k.setValue (def, juce::dontSendNotification);
}

void ShowcaseComponent::setupLabel (juce::Label& l, const juce::String& text)
{
    l.setText (text, juce::dontSendNotification);
    l.setFont (Fonts::body (13.f));
    l.setColour (juce::Label::textColourId, Colours::vhsCyan.withAlpha (0.85f));
    l.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (l);
}

// ---------------------------------------------------------------------------
void ShowcaseComponent::timerCallback()
{
    // Feed knob values → meters (simple linear mapping to 0..1)
    const float proNorm  = (float) knobPro_.valueToProportionOfLength    (knobPro_.getValue());
    const float neonNorm = (float) knobNeon_.valueToProportionOfLength   (knobNeon_.getValue());
    const float digNorm  = (float) knobDigital_.valueToProportionOfLength(knobDigital_.getValue());

    meterPro_.setLevel     (proNorm);
    meterNeon_.setLevel    (neonNorm);
    meterDigital_.setLevel (digNorm);
}

// ---------------------------------------------------------------------------
void ShowcaseComponent::paint (juce::Graphics& g)
{
    g.fillAll (Colours::vhsDark);

    // Title bar
    g.setFont (Fonts::heading (18.f));
    g.setColour (Colours::vhsCyan);
    g.drawText ("BoratoVHSStyle — Knob Showcase",
                0, 0, getWidth(), 36,
                juce::Justification::centred);
}

// ---------------------------------------------------------------------------
void ShowcaseComponent::resized()
{
    auto area = getLocalBounds();

    grid_.setBounds      (area);
    scanlines_.setBounds (area);
    noise_.setBounds     (area);

    // Three equal columns
    const int colW    = area.getWidth() / 3;
    const int padH    = 36 + 10; // title bar (36px) + gap
    const int knobSz  = juce::jmin (colW - 40, 160);
    const int meterH  = 90;
    const int labelH  = 22;
    const int meterW  = 18;

    auto layoutColumn = [&] (int col,
                              juce::Slider& knob,
                              bvs::VHSMeter& meter,
                              juce::Label& label)
    {
        const int cx     = col * colW + colW / 2;
        const int top    = padH;
        const int bottom = area.getHeight() - padH;

        // Knob centred horizontally
        knob.setBounds (cx - knobSz / 2, top, knobSz, knobSz);

        // Meter centred below knob
        const int meterTop = top + knobSz + 12;
        meter.setBounds (cx - meterW / 2, meterTop, meterW, meterH);

        // Label below meter
        label.setBounds (col * colW, meterTop + meterH + 6,
                         colW, labelH);

        juce::ignoreUnused (bottom);
    };

    layoutColumn (0, knobPro_,     meterPro_,     labelPro_);
    layoutColumn (1, knobNeon_,    meterNeon_,     labelNeon_);
    layoutColumn (2, knobDigital_, meterDigital_,  labelDigital_);

    // Pulse slider at the bottom
    const int sliderY  = area.getHeight() - 52;
    const int sliderW  = area.getWidth() - 80;

    pulseLabel_.setBounds (40, sliderY - 20, sliderW, 18);
    pulseSlider_.setBounds (40, sliderY, sliderW, 26);
}
