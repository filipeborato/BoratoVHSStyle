cmake_minimum_required(VERSION 3.20)
project(KnobShowcase VERSION 0.1 LANGUAGES CXX)

# Expects to be included from the root via add_subdirectory
# BoratoVHSStyle and JUCE targets are already available.

juce_add_gui_app(KnobShowcase
    PRODUCT_NAME     "VHS Knob Showcase"
    COMPANY_NAME     "Borato"
    BUNDLE_IDENTIFIER "com.borato.knob-showcase"
)

target_sources(KnobShowcase
    PRIVATE
        Source/Main.cpp
        Source/ShowcaseComponent.cpp
)

target_compile_features(KnobShowcase PRIVATE cxx_std_17)

target_link_libraries(KnobShowcase
    PRIVATE
        BoratoVHSStyle
        juce::juce_gui_basics
        juce::juce_graphics
        juce::juce_gui_extra
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)
