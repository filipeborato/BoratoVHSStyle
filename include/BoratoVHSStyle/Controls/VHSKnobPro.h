#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BoratoVHSStyle/Controls/KnobStyle.h"
#include "BoratoVHSStyle/Controls/KnobGeometry.h"

namespace bvs {

/**
 * @brief Production-quality rotary slider with radial-gradient body,
 *        animated progress arc, triangular pointer, graduated ticks,
 *        inner shadow simulation, specular highlight, and micro-texture.
 *
 * Usage:
 * @code
 *   bvs::VHSKnobPro knob;
 *   knob.setAccentColour (bvs::Colours::vhsMagenta);
 *   knob.setMarkerCount (11);
 *   addAndMakeVisible (knob);
 * @endcode
 *
 * The component owns its own LookAndFeel-independent paint path;
 * assign a VHSLookAndFeel only if you need consistent slider text-box styling.
 */
class VHSKnobPro : public juce::Slider
{
public:
    explicit VHSKnobPro (KnobStyle style = KnobStyle::Pro);
    ~VHSKnobPro() override = default;

    // ---- Visual configuration --------------------------------------------

    /** Overall glow intensity around the knob body [0, 1]. Default 0.6. */
    void setGlowStrength (float g)       noexcept;

    /** Number of tick marks on the outer ring [2, 24]. Default 11. */
    void setMarkerCount (int count)      noexcept;

    /** Accent colour used for the progress arc and pointer. */
    void setAccentColour (juce::Colour c) noexcept;

    /** Outer-ring stroke width in logical pixels [1, 12]. Default 3. */
    void setRingThickness (float px)     noexcept;

    /** Minimum and maximum brightness for the procedural micro-texture [0, 255]. */
    void setTextureBrightness (int minBright, int maxBright) noexcept
    {
        texBrightMin_ = minBright;
        texBrightMax_ = maxBright;
        textureDirty_ = true;
        repaint();
    }

    /** Maximum alpha for the micro-texture pixels [0, 1]. */
    void setTextureAlphaMax (float maxAlpha) noexcept
    {
        texAlphaMax_   = maxAlpha;
        textureDirty_  = true;
        repaint();
    }

    // ---- juce::Component --------------------------------------------------
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // ---- Config -----------------------------------------------------------
    KnobStyle   style_         { KnobStyle::Pro };
    float       glowStrength_  { 0.60f };
    int         markerCount_   { 11 };
    juce::Colour accentColour_;           // initialised in ctor
    float       ringThickness_ { 3.0f };

    // ---- Texture config (tunable by subclasses) ---------------------------
    int   texBrightMin_  {  4 };
    int   texBrightMax_  { 18 };
    float texAlphaMax_   { 0.012f };

    // ---- Geometry cache ---------------------------------------------------
    KnobGeometry geom_;
    juce::Path   cachedTickPaths_;
    bool         ticksDirty_   { true };

    // Micro-texture image (64×64, rebuilt only on resize)
    juce::Image  microTexture_;
    bool         textureDirty_ { true };

    // ---- Helpers ----------------------------------------------------------
    void rebuildTexture (int w, int h);
    void drawBody          (juce::Graphics&) const;
    void drawProgressArc   (juce::Graphics&) const;
    void drawTicks         (juce::Graphics&);
    void drawPointer       (juce::Graphics&) const;
    void drawSpecular      (juce::Graphics&) const;
    void drawGlowRing      (juce::Graphics&) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VHSKnobPro)
};

} // namespace bvs
