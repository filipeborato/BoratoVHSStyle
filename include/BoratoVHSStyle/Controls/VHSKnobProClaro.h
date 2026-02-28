#pragma once

#include "BoratoVHSStyle/Controls/VHSKnobPro.h"

namespace bvs {

/**
 * @brief Variante mais clara do VHSKnobPro.
 *
 * Idêntico ao VHSKnobPro em tudo, exceto pela micro-textura do corpo:
 * pixels mais brilhantes (40–90) com alpha ligeiramente maior (2%).
 * Útil para fundos mais escuros onde um grão visível agrega profundidade.
 *
 * @code
 *   bvs::VHSKnobProClaro knob;
 *   knob.setAccentColour (bvs::Colours::vhsCyan);
 * @endcode
 */
class VHSKnobProClaro : public VHSKnobPro
{
public:
    explicit VHSKnobProClaro (KnobStyle style = KnobStyle::Pro)
        : VHSKnobPro (style)
    {
        setTextureBrightness (40, 90);
        setTextureAlphaMax   (0.02f);
    }

    ~VHSKnobProClaro() override = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VHSKnobProClaro)
};

} // namespace bvs
