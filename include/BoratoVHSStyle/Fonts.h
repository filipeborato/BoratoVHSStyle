#pragma once

#include <juce_graphics/juce_graphics.h>

namespace bvs

{

struct Fonts

{

    // Config: nomes "humanos" + fallback

    static juce::Font heading(float px = 20.0f);

    static juce::Font body(float px = 14.0f);

    // Injeção de fontes custom via memória (ex: BinaryData)

    static void setHeadingTypefaceFromMemory(const void* data, size_t size);

    static void setBodyTypefaceFromMemory(const void* data, size_t size);

    // Tenta "Russo One", fallback sans

    static void setHeadingTypefaceName(juce::String name);

    // Tenta "Share Tech Mono", fallback mono

    static void setBodyTypefaceName(juce::String name);

private:

    static juce::Typeface::Ptr headingTf;

    static juce::Typeface::Ptr bodyTf;

    static juce::String headingName;

    static juce::String bodyName;

};

}
