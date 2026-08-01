cmake_minimum_required(VERSION 3.16)
project(audio_dsp_app LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(cpp)
add_subdirectory(juce)

message(STATUS "Build JUCE standalone app with JUCE_DIR set to your JUCE CMake package directory.")
