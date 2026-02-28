#include "BoratoVHSStyle/Controls/KnobGeometry.h"

#include <cmath>

namespace bvs {

// ---------------------------------------------------------------------------
static constexpr float kTwoPi = juce::MathConstants<float>::twoPi;

// ---------------------------------------------------------------------------
bool KnobGeometry::isDirty (juce::Rectangle<float> bounds,
                             float sliderPos,
                             float ringThickness) const noexcept
{
    return bounds       != lastBounds
        || sliderPos    != lastSliderPos
        || ringThickness != lastRingThickness;
}

// ---------------------------------------------------------------------------
void KnobGeometry::rebuild (juce::Rectangle<float> bounds,
                             float sliderPos,
                             float rotaryStart,
                             float rotaryEnd,
                             float ringThickness)
{
    lastBounds       = bounds;
    lastSliderPos    = sliderPos;
    lastRingThickness = ringThickness;

    // ---- derived scalars --------------------------------------------------
    const float diameter = juce::jmin (bounds.getWidth(), bounds.getHeight())
                           - ringThickness * 2.0f;

    radius       = diameter * 0.5f;
    centre       = bounds.getCentre();
    startAngle   = rotaryStart;
    endAngle     = rotaryEnd;
    currentAngle = rotaryStart + sliderPos * (rotaryEnd - rotaryStart);

    // ---- body rect (for radial gradient centre-filling) ------------------
    bodyRect = juce::Rectangle<float> (centre.x - radius,
                                       centre.y - radius,
                                       diameter, diameter);

    // ---- sweep arc (full range background) --------------------------------
    sweepArc.clear();
    sweepArc.addCentredArc (centre.x, centre.y,
                             radius, radius,
                             0.f,
                             rotaryStart, rotaryEnd,
                             true);

    // ---- progress arc (start → current) -----------------------------------
    progressArc.clear();

    // Only draw if there is a meaningful arc to render
    if (std::abs (currentAngle - startAngle) > 0.001f)
    {
        progressArc.addCentredArc (centre.x, centre.y,
                                   radius, radius,
                                   0.f,
                                   startAngle, currentAngle,
                                   true);
    }

    // ---- triangular pointer (centred on origin, pointing right/up) --------
    // The triangle is built in object space then rotated by currentAngle at
    // draw time via AffineTransform — avoids rebuilding on every value change.
    const float tipLen  = radius * 0.72f;
    const float baseW   = radius * 0.10f;

    pointerPath.clear();
    pointerPath.startNewSubPath (0.f, -tipLen);               // tip
    pointerPath.lineTo ( baseW,  0.f);                        // base-right
    pointerPath.lineTo (-baseW,  0.f);                        // base-left
    pointerPath.closeSubPath();
}

// ---------------------------------------------------------------------------
juce::Point<float> KnobGeometry::pointerTip (float lengthRatio) const noexcept
{
    return centre + juce::Point<float> (std::sin (currentAngle),
                                        -std::cos (currentAngle)) * (radius * lengthRatio);
}

// ---------------------------------------------------------------------------
juce::Point<float> KnobGeometry::specularPos (float radiusRatio) const noexcept
{
    // Specular highlight sits on the top-left quadrant of the knob, offset
    // slightly toward the light source (fixed upper-left).
    const float specAngle = currentAngle - juce::MathConstants<float>::halfPi * 0.6f;
    return centre + juce::Point<float> (std::sin (specAngle),
                                        -std::cos (specAngle)) * (radius * radiusRatio);
}

// ---------------------------------------------------------------------------
void KnobGeometry::buildTickPaths (juce::Path& dest,
                                   int markerCount,
                                   float innerRatio,
                                   float outerRatio) const
{
    dest.clear();

    if (markerCount < 2)
        return;

    for (int i = 0; i < markerCount; ++i)
    {
        const float t = static_cast<float> (i) / static_cast<float> (markerCount - 1);
        const float a = startAngle + t * (endAngle - startAngle);

        const float sinA = std::sin (a);
        const float cosA = std::cos (a);  // JUCE angles: 0 = top, CW

        const juce::Point<float> inner { centre.x + sinA * radius * innerRatio,
                                          centre.y - cosA * radius * innerRatio };
        const juce::Point<float> outer { centre.x + sinA * radius * outerRatio,
                                          centre.y - cosA * radius * outerRatio };

        dest.startNewSubPath (inner);
        dest.lineTo (outer);
    }
}

// ---------------------------------------------------------------------------
void KnobGeometry::buildSegmentPaths (juce::Array<juce::Path>& dest,
                                      int stepCount) const
{
    dest.clearQuick();

    if (stepCount < 2)
        return;

    const float totalAngle = endAngle - startAngle;
    const float segAngle   = totalAngle / static_cast<float> (stepCount);
    const float gap        = juce::jmin (segAngle * 0.18f, 0.04f); // small gap between segments

    for (int i = 0; i < stepCount; ++i)
    {
        const float segStart = startAngle + i * segAngle + gap * 0.5f;
        const float segEnd   = segStart + segAngle - gap;

        juce::Path p;
        p.addCentredArc (centre.x, centre.y,
                         radius, radius,
                         0.f,
                         segStart, segEnd,
                         true);
        dest.add (std::move (p));
    }
}

} // namespace bvs
