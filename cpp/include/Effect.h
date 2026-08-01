#pragma once

#include <functional>
#include <string>
#include <vector>

namespace audio {

constexpr int kMaxChannels = 2;

// Describes one user-facing knob so a UI can build controls generically
// without hand-wiring a panel per effect type.
struct ParamInfo {
    std::string label;
    float minValue;
    float maxValue;
    float defaultValue;
    std::string suffix;
    std::function<void(float)> setValue;
};

// Common interface for a single pedal/rack module in the signal chain.
// Audio is passed as an array of per-channel pointers (JUCE's native
// non-interleaved layout), so effects can hold independent per-channel state.
class Effect {
public:
    virtual ~Effect() = default;

    virtual void prepare(double sampleRate) = 0;
    virtual void reset() = 0;
    virtual void process(float* const* channelData, int numChannels, int numSamples) = 0;
    virtual const char* name() const = 0;
    virtual const char* category() const = 0;
    virtual std::vector<ParamInfo> getParameters() = 0;

    void setBypassed(bool shouldBypass) { bypassed = shouldBypass; }
    bool isBypassed() const { return bypassed; }

protected:
    bool bypassed = false;
};

} // namespace audio
