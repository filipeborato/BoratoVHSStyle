#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

class VHSGridBackground : public juce::Component
{
public:
    void setSpacing(int px); // default 30
    void paint(juce::Graphics&) override;

private:
    int spacing = 30;
};

}
