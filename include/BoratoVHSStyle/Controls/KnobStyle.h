#pragma once

namespace bvs {

/** Selects the visual personality of any VHS knob at construction time. */
enum class KnobStyle
{
    Basic,     /**< Original single-ring knob — backward-compatible. */
    Pro,       /**< Radial-gradient body, progress arc, graduated ticks, specular. */
    NeonRing,  /**< Minimalist dark body with dual-glow neon progress arc. */
    Digital    /**< Segmented encoder-style ring with centre value label. */
};

} // namespace bvs
