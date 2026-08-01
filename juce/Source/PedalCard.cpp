#include "PedalCard.h"
#include "PedalTexture.h"
#include "RigDesign.h"

PedalCard::PedalCard(const juce::String& title, const juce::String& categoryLabel, juce::Colour accentColour)
    : moduleTitle(title), categoryText(categoryLabel), accent(accentColour) {
    setRepaintsOnMouseActivity(true);
}

void PedalCard::setSelected(bool shouldBeSelected) {
    if (selected == shouldBeSelected) return;
    selected = shouldBeSelected;
    repaint();
}

void PedalCard::setEngaged(bool engaged) {
    if (engagedState == engaged) return;
    engagedState = engaged;
    repaint();
}

void PedalCard::setKnobs(std::vector<MiniKnob> newKnobs) {
    knobs = std::move(newKnobs);
    repaint();
}

int PedalCard::getPreferredWidth() const {
    const int count = static_cast<int>(juce::jmax<size_t>(knobs.size(), 1));
    return juce::jlimit(112, 236, 88 + count * 26);
}

juce::Rectangle<float> PedalCard::getSwitchBounds() const {
    auto bounds = getLocalBounds().toFloat();
    return juce::Rectangle<float>(38.0f, 38.0f).withCentre({ bounds.getCentreX(), bounds.getBottom() - 30.0f });
}

juce::Rectangle<float> PedalCard::getLedJewelBounds() const {
    auto bounds = getLocalBounds().toFloat();
    return juce::Rectangle<float>(9.0f, 9.0f).withCentre({ bounds.getCentreX(), bounds.getBottom() - 60.0f });
}

juce::Rectangle<float> PedalCard::getRemoveBounds() const {
    auto bounds = getLocalBounds().toFloat();
    return juce::Rectangle<float>(20.0f, 20.0f).withCentre({ bounds.getRight() - 15.0f, bounds.getY() + 15.0f });
}

juce::Rectangle<float> PedalCard::getKnobRowBounds() const {
    auto bounds = getLocalBounds().toFloat();
    return juce::Rectangle<float>(bounds.getWidth() - 16.0f, 48.0f).withCentre({ bounds.getCentreX(), 64.0f });
}

namespace {
void drawMiniKnob(juce::Graphics& g, juce::Rectangle<float> bounds, float normalized, juce::Colour accent, bool engaged) {
    constexpr float startAngle = -2.35619f;
    constexpr float endAngle = 2.35619f;
    const float angle = startAngle + juce::jlimit(0.0f, 1.0f, normalized) * (endAngle - startAngle);
    const auto centre = bounds.getCentre();
    const float radius = bounds.getWidth() * 0.5f;

    juce::ColourGradient body(juce::Colour(0xff54575f), centre.x, centre.y - radius,
                              juce::Colour(0xff222327), centre.x, centre.y + radius, false);
    g.setGradientFill(body);
    g.fillEllipse(bounds);
    g.setColour((engaged ? accent : juce::Colours::black).withAlpha(engaged ? 0.9f : 0.5f));
    g.drawEllipse(bounds, 1.1f);

    juce::Path pointer;
    pointer.startNewSubPath(centre.x, centre.y);
    pointer.lineTo(centre.x, centre.y - radius * 0.8f);
    g.setColour(juce::Colours::white.withAlpha(engaged ? 0.9f : 0.4f));
    g.strokePath(pointer, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded),
                 juce::AffineTransform::rotation(angle, centre.x, centre.y));
}
} // namespace

void PedalCard::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(1.5f);

    if (dragging) {
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(bounds.translated(0.0f, 6.0f), 8.0f);
    }

    if (selected) {
        g.setColour(accent.withAlpha(0.24f));
        g.fillRoundedRectangle(bounds.expanded(3.0f), 11.0f);
    }

    PedalTexture::paintEnclosure(g, bounds, accent, 8.0f, engagedState);

    if (selected) {
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 8.0f, 1.6f);
    }

    const float inset = 11.0f;
    PedalTexture::drawScrew(g, { bounds.getX() + inset, bounds.getY() + inset }, 3.2f);
    PedalTexture::drawScrew(g, { bounds.getRight() - inset, bounds.getY() + inset }, 3.2f);
    PedalTexture::drawScrew(g, { bounds.getX() + inset, bounds.getBottom() - inset }, 3.2f);
    PedalTexture::drawScrew(g, { bounds.getRight() - inset, bounds.getBottom() - inset }, 3.2f);

    // Silkscreen-style title: dark under-layer + bright top layer reads as
    // an engraved/printed label rather than flat UI text.
    auto textArea = juce::Rectangle<float>(bounds.getX() + 14.0f, bounds.getY() + 12.0f, bounds.getWidth() - 28.0f, 24.0f);
    const juce::Colour textColour = juce::Colours::white.withAlpha(engagedState ? 0.95f : 0.4f);
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.drawFittedText(moduleTitle.toUpperCase(), textArea.translated(0.0f, 1.3f).toNearestInt(),
                      juce::Justification::centred, 1);
    g.setColour(textColour);
    g.drawFittedText(moduleTitle.toUpperCase(), textArea.toNearestInt(), juce::Justification::centred, 1);

    // Real per-pedal controls, one small dial per actual parameter - this is
    // what makes a 1-knob gate look like a 1-knob gate and a 5-knob
    // compressor look like a bigger, busier box.
    if (!knobs.empty()) {
        auto row = getKnobRowBounds();
        const int count = static_cast<int>(knobs.size());
        const float slotWidth = row.getWidth() / static_cast<float>(count);
        const float diameter = juce::jlimit(16.0f, 32.0f, juce::jmin(slotWidth - 6.0f, row.getHeight()));
        for (int i = 0; i < count; ++i) {
            const auto slot = row.withX(row.getX() + slotWidth * static_cast<float>(i)).withWidth(slotWidth);
            const auto knobBounds = juce::Rectangle<float>(diameter, diameter).withCentre(slot.getCentre());
            drawMiniKnob(g, knobBounds, knobs[static_cast<size_t>(i)].normalized, accent, engagedState);
        }
    }

    // LED status jewel.
    const auto led = getLedJewelBounds();
    if (engagedState) {
        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.fillEllipse(led.expanded(4.5f));
    }
    juce::ColourGradient ledGrad(engagedState ? juce::Colours::white : juce::Colour(0xff3a2323),
                                  led.getX(), led.getY(),
                                  engagedState ? accent : juce::Colour(0xff1c1414), led.getRight(), led.getBottom(), true);
    g.setGradientFill(ledGrad);
    g.fillEllipse(led);
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawEllipse(led, 0.8f);

    // Chrome footswitch: outer ring + inner cap (flatter/darker when
    // bypassed, like a stomped-in switch).
    const auto sw = getSwitchBounds();
    juce::ColourGradient ring(juce::Colour(0xfff4f5f7), sw.getX(), sw.getY(),
                               juce::Colour(0xff53565d), sw.getRight(), sw.getBottom(), false);
    g.setGradientFill(ring);
    g.fillEllipse(sw);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(sw.reduced(0.5f), 1.0f);

    auto cap = sw.reduced(sw.getWidth() * (engagedState ? 0.24f : 0.16f));
    juce::ColourGradient capGrad(engagedState ? juce::Colour(0xff44474e) : juce::Colour(0xff222429),
                                  cap.getX(), cap.getY(),
                                  juce::Colour(0xff101114), cap.getX(), cap.getBottom(), false);
    g.setGradientFill(capGrad);
    g.fillEllipse(cap);
    g.setColour(juce::Colours::white.withAlpha(engagedState ? 0.18f : 0.08f));
    g.drawEllipse(cap.reduced(1.2f), 1.0f);

    if (hovered || selected) {
        const auto remove = getRemoveBounds();
        g.setColour(juce::Colour(0xff3a3d44));
        g.fillEllipse(remove);
        g.setColour(juce::Colour(RigDesign::kTextPrimary).withAlpha(0.85f));
        g.drawLine(remove.getX() + 6.0f, remove.getY() + 6.0f, remove.getRight() - 6.0f, remove.getBottom() - 6.0f, 1.6f);
        g.drawLine(remove.getRight() - 6.0f, remove.getY() + 6.0f, remove.getX() + 6.0f, remove.getBottom() - 6.0f, 1.6f);
    }
}

void PedalCard::resized() {}

void PedalCard::mouseDown(const juce::MouseEvent& e) {
    dragging = false;
    if (getSwitchBounds().contains(e.position) || getRemoveBounds().contains(e.position))
        return;
    if (onDragStart) onDragStart(e);
}

void PedalCard::mouseDrag(const juce::MouseEvent& e) {
    const auto downPos = e.getMouseDownPosition().toFloat();
    if (getSwitchBounds().contains(downPos) || getRemoveBounds().contains(downPos))
        return;
    if (!dragging && e.getDistanceFromDragStart() > 6) {
        dragging = true;
        repaint();
    }
    if (dragging && onDragMove) onDragMove(e);
}

void PedalCard::mouseUp(const juce::MouseEvent& e) {
    const auto downPos = e.getMouseDownPosition().toFloat();
    if (getSwitchBounds().contains(downPos)) {
        engagedState = !engagedState;
        repaint();
        if (onBypassToggle) onBypassToggle(engagedState);
        return;
    }
    if (getRemoveBounds().contains(downPos)) {
        if (onRemove) onRemove();
        return;
    }
    if (dragging) {
        dragging = false;
        repaint();
        if (onDragEnd) onDragEnd(e);
    } else {
        if (onSelect) onSelect();
    }
}
