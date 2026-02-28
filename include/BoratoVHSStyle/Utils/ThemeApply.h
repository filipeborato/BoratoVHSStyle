#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "BoratoVHSStyle/ThemeConfig.h"

#include "BoratoVHSStyle/Controls/VHSLookAndFeel.h"

namespace bvs {

void applyThemeToTree(juce::Component& root, VHSLookAndFeel& laf, const ThemeConfig& cfg);

}
