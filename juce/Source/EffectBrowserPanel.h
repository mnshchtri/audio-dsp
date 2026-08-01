#pragma once

#include <JuceHeader.h>
#include "EffectFactory.h"
#include <functional>

// Slide-in drawer listing every registered effect type grouped by category
// (Dynamics/Drive/Amp/Modulation/Time/Reverb) so the user can add pedals to
// the chain the way Neural DSP's plugins let you browse and drop in effects.
class EffectBrowserPanel : public juce::Component {
public:
    EffectBrowserPanel();

    std::function<void(audio::EffectType)> onEffectChosen;
    std::function<void()> onCloseRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    class EffectTile : public juce::Component {
    public:
        EffectTile(audio::EffectType t, const juce::String& title, juce::Colour accentColour);
        void paint(juce::Graphics& g) override;
        void mouseEnter(const juce::MouseEvent&) override { hovered = true; repaint(); }
        void mouseExit(const juce::MouseEvent&) override { hovered = false; repaint(); }
        void mouseUp(const juce::MouseEvent&) override { if (onClick) onClick(); }

        audio::EffectType type;
        std::function<void()> onClick;

    private:
        juce::String name;
        juce::Colour accent;
        bool hovered = false;
    };

    juce::Label titleLabel;
    juce::TextButton closeButton;
    juce::Viewport viewport;
    juce::Component listContent;
    juce::OwnedArray<juce::Label> categoryLabels;
    juce::OwnedArray<EffectTile> tiles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectBrowserPanel)
};
