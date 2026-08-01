#pragma once

#include <JuceHeader.h>

// Dark, premium rack-unit look and feel: brushed-metal knobs with a glowing
// arc readout, footswitch-style toggle with LED, and shared card/glow
// drawing helpers used across the chain strip, browser, and module editor.
class RigLookAndFeel : public juce::LookAndFeel_V4 {
public:
    RigLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY,
                      int buttonW, int buttonH, juce::ComboBox& box) override;

    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;

    static void drawCard(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour accent,
                          bool glow, float cornerSize = 12.0f);

    static const juce::Colour accentColour;
    static const juce::Colour panelColour;
    static const juce::Colour panelColourAlt;
};
