cmake_minimum_required(VERSION 3.16)
project(audio_dsp_juce LANGUAGES CXX)

if(NOT DEFINED JUCE_DIR)
    message(FATAL_ERROR "Please set JUCE_DIR to your JUCE checkout path before configuring this project.")
endif()

add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE)

juce_add_gui_app(AudioDSPApp
    COMPANY_NAME "AudioDSP"
    PRODUCT_NAME "Audio DSP Guitar Rig"
    VERSION 1.0.0
    BUNDLE_ID "com.audiodsp.guitarrig"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This app needs microphone/audio-interface input to process your guitar signal."
)

target_sources(AudioDSPApp PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/MainComponent.h
    Source/ChainStripComponent.cpp
    Source/ChainStripComponent.h
    Source/EffectBrowserPanel.cpp
    Source/EffectBrowserPanel.h
    Source/ModuleEditorPanel.cpp
    Source/ModuleEditorPanel.h
    Source/PedalCard.cpp
    Source/PedalCard.h
    Source/PresetBar.cpp
    Source/PresetBar.h
    Source/RigDesign.h
    Source/RigLookAndFeel.cpp
    Source/RigLookAndFeel.h
    Source/LevelMeter.h
)

target_compile_definitions(AudioDSPApp PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(AudioDSPApp
    PRIVATE
        audio_engine
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(AudioDSPApp)
