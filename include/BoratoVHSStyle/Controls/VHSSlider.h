#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSSlider : public juce::Slider {
public:
    explicit VHSSlider(bool horizontal = true)
    {
        setSliderStyle(horizontal ? LinearHorizontal : LinearVertical);
        setTextBoxStyle(NoTextBox, false, 0, 0);
    }
};

}
