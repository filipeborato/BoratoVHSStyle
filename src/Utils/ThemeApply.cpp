#include "BoratoVHSStyle/Utils/ThemeApply.h"

#include "BoratoVHSStyle/Colours.h"

namespace bvs {

static void walk(juce::Component& c, VHSLookAndFeel& laf)

{

    c.setLookAndFeel(&laf);

    for (int i=0;i<c.getNumChildComponents();++i) walk(*c.getChildComponent(i), laf);

}

void applyThemeToTree(juce::Component& root, VHSLookAndFeel& laf, const ThemeConfig&)

{

    walk(root, laf);

    root.repaint();

}

}
