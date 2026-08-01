#include "PluginEditor.h"
#include <iostream>

namespace audio {

PluginEditor::PluginEditor() = default;

void PluginEditor::sendUiMessage(const char* name, float value) {
    std::cout << "UI message: " << name << " = " << value << "\n";
}

} // namespace audio
