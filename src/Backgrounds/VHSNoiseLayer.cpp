#include "BoratoVHSStyle/Backgrounds/VHSNoiseLayer.h"

#include <random>

namespace bvs {

VHSNoiseLayer::VHSNoiseLayer() { setFPS(30); }

void VHSNoiseLayer::setAmount(float a){ amount = juce::jlimit(0.f,1.f,a); }

void VHSNoiseLayer::setFPS(int fps){ startTimerHz(juce::jlimit(1,120,fps)); }

void VHSNoiseLayer::timerCallback(){ regenerate(); repaint(); }

void VHSNoiseLayer::regenerate()
{
    noise = juce::Image(juce::Image::RGB, 64, 64, false);

    std::mt19937 rng((uint32_t)juce::Time::getMillisecondCounter());

    // Pixels escuros (0-80): grain de fita VHS real é sutil e escuro, não branco.
    // Para ruído mais claro use setAmount() — o alpha já controla a intensidade.
    std::uniform_int_distribution<int> dist(0, 80);

    juce::Graphics g(noise);

    for (int y = 0; y < noise.getHeight(); ++y)
        for (int x = 0; x < noise.getWidth(); ++x)
        {
            const auto v = (uint8_t)dist(rng);
            const auto c = juce::Colour(v, v, v).withAlpha(amount);
            g.setColour(c);
            g.fillRect(x, y, 1, 1);
        }
}

void VHSNoiseLayer::paint(juce::Graphics& g)
{
    g.setOpacity(1.0f);
    g.drawImageTransformed(noise, juce::AffineTransform::scale(
        (float)getWidth()  / noise.getWidth(),
        (float)getHeight() / noise.getHeight()));
}

} // namespace bvs
