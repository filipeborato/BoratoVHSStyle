#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace bvs {

/**
 * @brief Cached geometry for rotary sliders.
 *
 * All expensive angular math is performed once in rebuild() and reused
 * across paint passes.  Call isDirty() before paint() to decide whether
 * a rebuild is necessary.
 *
 * The struct is intentionally copy-constructible so components can keep
 * a prev-frame copy for dirty-checking without an extra heap allocation.
 */
struct KnobGeometry
{
    // ------------------------------------------------------------------ data
    juce::Point<float> centre;
    float radius        { 0.f };
    float startAngle    { 0.f };
    float endAngle      { 0.f };
    float currentAngle  { 0.f };

    /** Full-sweep background arc (entire range). */
    juce::Path sweepArc;

    /** Progress arc from startAngle to currentAngle. */
    juce::Path progressArc;

    /** Triangular pointer path centred on the origin — apply
     *  AffineTransform to rotate/translate before drawing. */
    juce::Path pointerPath;

    /** Small inner body circle. */
    juce::Rectangle<float> bodyRect;

    // ------------------------------------------------------------------ cache keys
    juce::Rectangle<float> lastBounds;
    float lastSliderPos { -1.f };
    float lastRingThickness { 0.f };

    // ---------------------------------------------------------------- interface

    /**
     * Recomputes all geometry.
     *
     * @param bounds         Component local bounds (toFloat()).
     * @param sliderPos      Normalised slider position [0, 1].
     * @param rotaryStart    Rotary start angle (radians, JUCE convention).
     * @param rotaryEnd      Rotary end angle (radians, JUCE convention).
     * @param ringThickness  Outer ring stroke width in logical pixels.
     */
    void rebuild (juce::Rectangle<float> bounds,
                  float sliderPos,
                  float rotaryStart,
                  float rotaryEnd,
                  float ringThickness);

    /**
     * Returns true when any input parameter has changed since the last
     * rebuild(), meaning the cached paths are stale.
     */
    bool isDirty (juce::Rectangle<float> bounds,
                  float sliderPos,
                  float ringThickness) const noexcept;

    // ------------------------------------------------------------------ helpers

    /** Centre + direction-vector tip of the pointer at currentAngle. */
    juce::Point<float> pointerTip (float lengthRatio = 0.75f) const noexcept;

    /** Position of the specular highlight ellipse. */
    juce::Point<float> specularPos (float radiusRatio = 0.45f) const noexcept;

    /** Returns tick positions (inner/outer pairs) for markerCount evenly-spaced ticks. */
    void buildTickPaths (juce::Path& dest, int markerCount,
                         float innerRatio = 0.78f,
                         float outerRatio = 0.92f) const;

    /**
     * Builds NeonRing segment arcs for VHSKnobDigital.
     * Each segment is a separate sub-path so the caller can colour each one.
     * Segments are stored in order (index 0 = first segment from startAngle).
     */
    void buildSegmentPaths (juce::Array<juce::Path>& dest,
                            int stepCount) const;
};

} // namespace bvs
