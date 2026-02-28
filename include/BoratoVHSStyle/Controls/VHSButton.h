#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSToggle : public juce::ToggleButton {

public: using juce::ToggleButton::ToggleButton; };

class VHSMomentary : public juce::TextButton {

public:

    VHSMomentary(juce::String text = {}) : juce::TextButton(std::move(text)) {

        setClickingTogglesState(false);

    }

};

}
