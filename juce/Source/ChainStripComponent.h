#pragma once

#include <JuceHeader.h>
#include "GuitarRigEngine.h"
#include <functional>
#include <memory>

// Horizontal, scrollable strip showing the live signal chain as a row of
// PedalCards: click to select (drives the big module editor), drag to
// reorder, LED to bypass, "x" to remove, "+" at the end to open the browser.
class ChainStripComponent : public juce::Component {
public:
    explicit ChainStripComponent(audio::GuitarRigEngine& engineToUse);
    ~ChainStripComponent() override;

    std::function<void(int)> onSelectionChanged;
    std::function<void()> onChainChanged;
    std::function<void()> onAddRequested;

    // Rebuilds the card list from the engine's current chain. Call after any
    // external mutation (browser add) or on startup.
    void refresh();
    // Re-reads the currently-selected pedal's live parameter values into its
    // mini-knob display, without a full rebuild (called as the big editor's
    // knobs move so the chain-strip card stays in sync).
    void refreshSelectedCardKnobs();
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return selectedIndex; }

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    class Content;
    friend class Content;

    audio::GuitarRigEngine& engine;
    juce::Viewport viewport;
    std::unique_ptr<Content> content;
    int selectedIndex = -1;
};
