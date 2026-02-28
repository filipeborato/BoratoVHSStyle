/*
  ==============================================================================

    This file exposes BoratoVHSStyle as a JUCE module so Projucer exporters can
    add the correct include paths and compile units in generated IDE projects.

  ==============================================================================
*/

#pragma once
#define BORATO_VHSSTYLE_H_INCLUDED

/*
  BEGIN_JUCE_MODULE_DECLARATION

   ID:               borato_vhsstyle
   vendor:           borato
   version:          0.1.0
   name:             Borato VHS Style
   description:      Retro VHS-style UI components for JUCE plug-ins
   website:          https://github.com/borato/BoratoVHSStyle
   license:          Proprietary
   minimumCppStandard: 17
   dependencies:     juce_core juce_graphics juce_gui_basics juce_gui_extra
   searchpaths:      ../../include

  END_JUCE_MODULE_DECLARATION
*/

#include <BoratoVHSStyle/BoratoVHSStyle.h>
