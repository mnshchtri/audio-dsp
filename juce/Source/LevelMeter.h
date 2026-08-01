#pragma once

#include <JuceHeader.h>
#include <atomic>

// Vertical peak meter with a decaying peak-hold line. pushLevel() is safe to
// call from the audio thread; painting/decay happens on the message thread.
class LevelMeter : public juce::Component, private juce::Timer {
public:
    LevelMeter() { startTimerHz(30); }

    void pushLevel(float peak) {
        float current = latestPeak.load(std::memory_order_relaxed);
        if (peak > current)
            latestPeak.store(peak, std::memory_order_relaxed);
    }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff101114));
        g.fillRoundedRectangle(bounds, 4.0f);

        const float level = juce::jlimit(0.0f, 1.0f, displayedLevel);
        auto meterBounds = bounds.reduced(2.0f);
        auto filled = meterBounds.removeFromBottom(meterBounds.getHeight() * level);

        juce::ColourGradient gradient(juce::Colour(0xff5CFF7A), 0, bounds.getBottom(),
                                       juce::Colour(0xffFF4D4D), 0, bounds.getY(), false);
        gradient.addColour(0.7, juce::Colour(0xffFFD24D));
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(filled, 3.0f);

        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 1.0f);
    }

private:
    void timerCallback() override {
        const float target = latestPeak.exchange(0.0f, std::memory_order_relaxed);
        if (target > displayedLevel)
            displayedLevel = target;
        else
            displayedLevel *= 0.85f;
        repaint();
    }

    std::atomic<float> latestPeak { 0.0f };
    float displayedLevel = 0.0f;
};
