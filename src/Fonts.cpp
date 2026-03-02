#include "BoratoVHSStyle/Fonts.h"

namespace bvs {

juce::Typeface::Ptr Fonts::headingTf;
juce::Typeface::Ptr Fonts::bodyTf;
juce::String Fonts::headingName = "Russo One";
juce::String Fonts::bodyName    = "Share Tech Mono";

static juce::Font makeFont(juce::Typeface::Ptr& tf,
                           const juce::String& prefName, float px,
                           bool monoFallback)
{
    const auto defaultStyle = juce::Font::getDefaultStyle();

    if (tf)
        return juce::Font(juce::FontOptions(tf).withPointHeight(px));

    auto f = juce::Font(juce::FontOptions(px).withName(prefName).withStyle(defaultStyle));

    if (! f.getTypefacePtr())
    {
        f = juce::Font(juce::FontOptions(px).withName(monoFallback ? juce::Font::getDefaultMonospacedFontName()
                                                                   : juce::Font::getDefaultSansSerifFontName())
                                            .withStyle(defaultStyle));
    }

    return f;
}

juce::Font Fonts::heading(float px) { return makeFont(headingTf, headingName, px, false); }
juce::Font Fonts::body   (float px) { return makeFont(bodyTf,    bodyName,    px, true ); }

void Fonts::setHeadingTypefaceFromMemory(const void* d, size_t s)
{ headingTf = juce::Typeface::createSystemTypefaceFor(d, (int)s); }

void Fonts::setBodyTypefaceFromMemory(const void* d, size_t s)
{ bodyTf = juce::Typeface::createSystemTypefaceFor(d, (int)s); }

void Fonts::setHeadingTypefaceName(juce::String n) { headingName = std::move(n); headingTf = nullptr; }
void Fonts::setBodyTypefaceName   (juce::String n) { bodyName    = std::move(n); bodyTf = nullptr; }

} // namespace
