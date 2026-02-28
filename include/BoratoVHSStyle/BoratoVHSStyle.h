#pragma once

// ---- Theme foundation --------------------------------------------------------
#include "ThemeConfig.h"
#include "Colours.h"
#include "Fonts.h"

// ---- Backgrounds -------------------------------------------------------------
#include "Backgrounds/VHSGridBackground.h"
#include "Backgrounds/ScanlinesOverlay.h"
#include "Backgrounds/VHSNoiseLayer.h"

// ---- Controls: knob system ---------------------------------------------------
#include "Controls/KnobStyle.h"
#include "Controls/KnobGeometry.h"

// ---- Controls: base look-and-feel -------------------------------------------
#include "Controls/VHSLookAndFeel.h"

// ---- Controls: knob variants -------------------------------------------------
#include "Controls/VHSKnob.h"          // Basic — original knob
#include "Controls/VHSKnobPro.h"       // Pro — radial gradient + progress arc
#include "Controls/VHSKnobProClaro.h"  // ProClaro — variante com textura mais clara
#include "Controls/VHSKnobNeonRing.h"  // NeonRing — dual-glow neon + chromatic aberration
#include "Controls/VHSKnobDigital.h"   // Digital — segmented encoder + centre label

// ---- Controls: other widgets -------------------------------------------------
#include "Controls/VHSSlider.h"
#include "Controls/VHSButton.h"
#include "Controls/VHSMeter.h"

// ---- Utilities ---------------------------------------------------------------
#include "Utils/ThemeApply.h"
#include "Utils/AudioReactive.h"
