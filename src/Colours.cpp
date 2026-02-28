#include "BoratoVHSStyle/Colours.h"

namespace bvs::Colours {

juce::ColourGradient diagonalGradient(juce::Rectangle<float> b)

{

    juce::ColourGradient gr(vhsPurple, b.getTopLeft(),

                            vhsBlue,   b.getBottomRight(), false);

    gr.addColour(0.5, vhsMagenta.withAlpha(0.8f));

    return gr;

}

void drawGlow(juce::Graphics& g, juce::Rectangle<float> r, juce::Colour c,

              float strength, int passes)

{

    auto img = juce::Image(juce::Image::ARGB, (int)r.getWidth(), (int)r.getHeight(), true);

    juce::Graphics ig(img);

    ig.setColour(c.withAlpha(0.35f));

    ig.fillEllipse(img.getBounds().toFloat());

    for (int i=0; i<passes; ++i)

        img = img.rescaled(img.getWidth()+4, img.getHeight()+4, juce::Graphics::highResamplingQuality);

    g.setOpacity(juce::jlimit(0.f, 1.f, strength));

    g.drawImageAt(img, (int)(r.getCentreX() - img.getWidth() * 0.5f),

                       (int)(r.getCentreY() - img.getHeight()* 0.5f));

}

}
