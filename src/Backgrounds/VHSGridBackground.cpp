#include "BoratoVHSStyle/Backgrounds/VHSGridBackground.h"

#include "BoratoVHSStyle/Colours.h"

namespace bvs {

void VHSGridBackground::setSpacing(int px){ spacing = juce::jmax(8, px); repaint(); }

void VHSGridBackground::paint(juce::Graphics& g)

{

    auto r = getLocalBounds().toFloat();

    g.setGradientFill(Colours::diagonalGradient(r));

    g.fillAll();

    g.setColour(Colours::vhsGrid.withAlpha(0.25f));

    for (int x = 0; x < getWidth(); x += spacing) g.drawVerticalLine(x, 0.f, (float)getHeight());

    for (int y = 0; y < getHeight(); y += spacing) g.drawHorizontalLine(y, 0.f, (float)getWidth());

}

}
