cmake_minimum_required(VERSION 3.20)

project(BoratoVHSStyle VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# JUCE 8
# ---------------------------------------------------------------------------
option(BVS_FETCH_JUCE "Download JUCE automatically when it is not installed" ON)

set(BVS_JUCE_SOURCE_DIR "" CACHE PATH "Path to a JUCE source checkout")

if(NOT BVS_JUCE_SOURCE_DIR AND EXISTS "C:/JUCE/CMakeLists.txt")
    set(BVS_JUCE_SOURCE_DIR "C:/JUCE")
endif()

if(BVS_JUCE_SOURCE_DIR AND EXISTS "${BVS_JUCE_SOURCE_DIR}/CMakeLists.txt")
    # Read the version from the local checkout so we can fail fast for incompatible JUCE.
    file(STRINGS "${BVS_JUCE_SOURCE_DIR}/CMakeLists.txt" _juce_ver_line
         REGEX "^[ \t]*project\\(JUCE[ \t]" LIMIT_COUNT 1)
    if(_juce_ver_line MATCHES "VERSION[ \t]+([0-9]+)")
        set(_juce_major ${CMAKE_MATCH_1})
        if(_juce_major LESS 8)
            message(FATAL_ERROR
                "BVS_JUCE_SOURCE_DIR points to JUCE ${_juce_major}.x, "
                "but BoratoVHSStyle requires JUCE 8. "
                "Please update BVS_JUCE_SOURCE_DIR to a JUCE 8 checkout.")
        endif()
    endif()
    set(JUCE_BUILD_HELPER_TOOLS ON CACHE BOOL "Build JUCE helper tools in-tree" FORCE)
    add_subdirectory("${BVS_JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/juce" EXCLUDE_FROM_ALL)
else()
    find_package(JUCE 8 CONFIG QUIET)

    if(NOT JUCE_FOUND)
        if(NOT BVS_FETCH_JUCE)
            message(FATAL_ERROR
                "JUCE was not found. Set BVS_JUCE_SOURCE_DIR/JUCE_DIR/CMAKE_PREFIX_PATH or enable BVS_FETCH_JUCE.")
        endif()

        include(FetchContent)

        FetchContent_Declare(
            juce
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG        8.0.7
            GIT_SHALLOW    TRUE
        )

        FetchContent_MakeAvailable(juce)
    endif()
endif()

# ---------------------------------------------------------------------------
# Optional OpenGL (noise layer shader) — disabled by default, not required
# ---------------------------------------------------------------------------
option(BVS_ENABLE_OPENGL "Enable OpenGL shader for VHS noise" OFF)

# ---------------------------------------------------------------------------
# BoratoVHSStyle static library
# ---------------------------------------------------------------------------
add_library(BoratoVHSStyle STATIC

    src/ThemeConfig.cpp
    src/Colours.cpp
    src/Fonts.cpp

    src/Backgrounds/VHSGridBackground.cpp
    src/Backgrounds/ScanlinesOverlay.cpp
    src/Backgrounds/VHSNoiseLayer.cpp

    src/Controls/VHSLookAndFeel.cpp
    src/Controls/KnobGeometry.cpp
    src/Controls/VHSKnob.cpp
    src/Controls/VHSKnobPro.cpp
    src/Controls/VHSKnobNeonRing.cpp
    src/Controls/VHSKnobDigital.cpp
    src/Controls/VHSSlider.cpp
    src/Controls/VHSButton.cpp
    src/Controls/VHSMeter.cpp

    src/Utils/ThemeApply.cpp
    src/Utils/AudioReactive.cpp
)

target_include_directories(BoratoVHSStyle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(BoratoVHSStyle
    PUBLIC
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_gui_extra
    PRIVATE
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

if(BVS_ENABLE_OPENGL)
    target_compile_definitions(BoratoVHSStyle PRIVATE BVS_HAS_OPENGL=1)
    target_link_libraries(BoratoVHSStyle PRIVATE juce::juce_opengl)
endif()

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
option(BVS_BUILD_EXAMPLE  "Build demo plugin"              ON)
option(BVS_BUILD_SHOWCASE "Build KnobShowcase standalone"  ON)

if(BVS_BUILD_EXAMPLE)
    add_subdirectory(examples/DemoPlugin)
endif()

if(BVS_BUILD_SHOWCASE)
    add_subdirectory(examples/KnobShowcase)
endif()
