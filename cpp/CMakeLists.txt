cmake_minimum_required(VERSION 3.16)
project(audio_dsp_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(audio_engine STATIC
    src/SynthEngine.cpp
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
)

target_include_directories(audio_engine PUBLIC include)

# VST3 SDK path must be set externally for a real plugin build.
if(DEFINED VST3_SDK_PATH)
    message(STATUS "VST3 SDK found at ${VST3_SDK_PATH}")
    target_compile_definitions(audio_engine PRIVATE USE_VST3_SDK)
    target_include_directories(audio_engine PRIVATE ${VST3_SDK_PATH}/public.sdk/source/vst2.x)
endif()
