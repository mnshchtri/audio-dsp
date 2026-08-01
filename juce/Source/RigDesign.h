#pragma once

#include <JuceHeader.h>

// Shared design tokens: one accent colour per effect category, used across
// the chain strip, browser, and module editor so the whole app reads as one
// coherent, colour-coded rig rather than a pile of ad-hoc panels.
namespace RigDesign {

inline juce::Colour colourForCategory(const juce::String& category) {
    if (category == "Dynamics") return juce::Colour(0xff4FC3F7);
    if (category == "Drive") return juce::Colour(0xffFF7A45);
    if (category == "Amp") return juce::Colour(0xffFF4D6D);
    if (category == "Modulation") return juce::Colour(0xff2DD4BF);
    if (category == "Time") return juce::Colour(0xff4ADE80);
    if (category == "Reverb") return juce::Colour(0xffB98CFF);
    return juce::Colour(0xffB0B4BB);
}

inline const juce::StringArray& categoryOrder() {
    static const juce::StringArray order { "Dynamics", "Drive", "Amp", "Modulation", "Time", "Reverb" };
    return order;
}

constexpr auto kBackgroundTop = 0xff1c1e24;
constexpr auto kBackgroundBottom = 0xff101115;
constexpr auto kPanelColour = 0xff23252b;
constexpr auto kPanelColourAlt = 0xff2b2e35;
constexpr auto kBorderColour = 0xff0a0b0d;
constexpr auto kTextPrimary = 0xffeceef2;
constexpr auto kTextSecondary = 0xff8d919c;

} // namespace RigDesign
