#pragma once

namespace audio {

class PluginEditor {
public:
    PluginEditor();
    void sendUiMessage(const char* name, float value);
};

} // namespace audio
