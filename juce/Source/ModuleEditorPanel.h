#pragma once

#include <JuceHeader.h>
#include "GuitarRigEngine.h"
#include <functional>
#include <memory>
#include <vector>

// Large, focused editor for whichever module is selected in the chain
// strip: category-coloured header, bypass switch, and one big knob per
// parameter built generically from Effect::getParameters(). Shows a "no
// module selected" placeholder when the chain is empty.
class ModuleEditorPanel : public juce::Component {
public:
    ModuleEditorPanel();

    std::function<void(bool engaged)> onBypassChanged;
    std::function<void()> onParameterChanged;

    // effect may be nullptr to show the empty-state placeholder.
    void showEffect(audio::Effect* effect);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    struct Knob {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::Label> label;
    };

    juce::Label nameLabel;
    juce::Label categoryLabel;
    juce::ToggleButton bypassButton;
    std::vector<Knob> knobs;
    juce::Colour accent { 0xffB0B4BB };
    bool hasEffect = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleEditorPanel)
};
