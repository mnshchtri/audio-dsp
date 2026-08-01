#include "RigLookAndFeel.h"
#include "RigDesign.h"

const juce::Colour RigLookAndFeel::accentColour { 0xffFF7A45 };
const juce::Colour RigLookAndFeel::panelColour { RigDesign::kPanelColour };
const juce::Colour RigLookAndFeel::panelColourAlt { RigDesign::kPanelColourAlt };

RigLookAndFeel::RigLookAndFeel() {
    setColour(juce::Slider::thumbColourId, accentColour);
    setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff0d0e11));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::ToggleButton::textColourId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(RigDesign::kBackgroundBottom));
    setColour(juce::TextButton::buttonColourId, panelColourAlt);
    setColour(juce::TextButton::textColourOffId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::TextButton::textColourOnId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::ScrollBar::thumbColourId, juce::Colour(0xff4a4e58));
    setColour(juce::ComboBox::backgroundColourId, panelColourAlt);
    setColour(juce::ComboBox::textColourId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(RigDesign::kBorderColour));
    setColour(juce::ComboBox::arrowColourId, accentColour);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff23252b));
    setColour(juce::PopupMenu::textColourId, juce::Colour(RigDesign::kTextPrimary));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, accentColour.withAlpha(0.35f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

juce::Font RigLookAndFeel::getLabelFont(juce::Label&) {
    return juce::Font(juce::FontOptions(12.5f, juce::Font::bold));
}

juce::Font RigLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight) {
    return juce::Font(juce::FontOptions(juce::jmin(15.0f, static_cast<float>(buttonHeight) * 0.5f), juce::Font::bold));
}

void RigLookAndFeel::drawCard(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour accent,
                               bool glow, float cornerSize) {
    if (glow) {
        g.setColour(accent.withAlpha(0.16f));
        g.fillRoundedRectangle(bounds.expanded(4.0f), cornerSize + 3.0f);
    }

    juce::ColourGradient bg(juce::Colour(RigDesign::kPanelColourAlt), bounds.getX(), bounds.getY(),
                             juce::Colour(RigDesign::kPanelColour), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.setColour(glow ? accent.withAlpha(0.8f) : juce::Colour(RigDesign::kBorderColour));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, glow ? 1.6f : 1.0f);
}

void RigLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider& slider) {
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                                static_cast<float>(width), static_cast<float>(height))
                             .reduced(4.0f);
    const float diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
    const auto knobBounds = juce::Rectangle<float>(diameter, diameter).withCentre(bounds.getCentre());
    const float outerRadius = diameter * 0.5f;
    const float centreX = knobBounds.getCentreX();
    const float centreY = knobBounds.getCentreY();
    const juce::Point<float> centre(centreX, centreY);
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const juce::Colour accent = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // Printed gauge tick marks, like the numbers stencilled around a vintage
    // pedal's control.
    constexpr int numTicks = 11;
    for (int i = 0; i < numTicks; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(numTicks - 1);
        const float tickAngle = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
        const auto p1 = centre.getPointOnCircumference(outerRadius * 0.99f, tickAngle);
        const auto p2 = centre.getPointOnCircumference(outerRadius * 0.86f, tickAngle);
        g.setColour(juce::Colours::white.withAlpha(0.32f));
        g.drawLine({ p1, p2 }, i % 5 == 0 ? 1.8f : 1.1f);
    }

    // Thin backlit value arc: the modern touch against the vintage body.
    const float arcRadius = outerRadius * 0.80f;
    juce::Path track;
    track.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff101114));
    g.strokePath(track, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(accent);
    g.strokePath(valueArc, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Heavy-knurled knob body: ridge marks drawn first, the metal disc
    // painted on top so only the ridge tips peek out past the rim.
    const float bodyRadius = outerRadius * 0.64f;
    const auto bodyBounds = juce::Rectangle<float>(bodyRadius * 2.0f, bodyRadius * 2.0f).withCentre(centre);

    constexpr int numRidges = 32;
    for (int i = 0; i < numRidges; ++i) {
        const float ridgeAngle = static_cast<float>(i) / static_cast<float>(numRidges) * juce::MathConstants<float>::twoPi;
        const auto p1 = centre.getPointOnCircumference(bodyRadius * 1.05f, ridgeAngle);
        const auto p2 = centre.getPointOnCircumference(bodyRadius * 0.78f, ridgeAngle);
        g.setColour(i % 2 == 0 ? juce::Colours::white.withAlpha(0.14f) : juce::Colours::black.withAlpha(0.30f));
        g.drawLine({ p1, p2 }, 1.7f);
    }

    const auto discBounds = bodyBounds.reduced(bodyRadius * 0.16f);
    juce::ColourGradient bodyGradient(juce::Colour(0xff53565e), centreX, centreY - bodyRadius,
                                       juce::Colour(0xff1d1e22), centreX, centreY + bodyRadius, false);
    g.setGradientFill(bodyGradient);
    g.fillEllipse(discBounds);

    juce::ColourGradient gloss(juce::Colours::white.withAlpha(0.30f), centreX - bodyRadius * 0.35f, centreY - bodyRadius * 0.55f,
                                juce::Colours::white.withAlpha(0.0f), centreX, centreY, false);
    g.setGradientFill(gloss);
    g.fillEllipse(discBounds);

    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawEllipse(discBounds, 1.0f);

    // Pointer line + tip dot, rotated the same way the value arc sweeps.
    juce::Path pointer;
    const float pointerLength = bodyRadius * 0.72f;
    pointer.startNewSubPath(centreX, centreY);
    pointer.lineTo(centreX, centreY - pointerLength);
    g.setColour(juce::Colours::white.withAlpha(0.95f));
    g.strokePath(pointer, juce::PathStrokeType(2.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded),
                 juce::AffineTransform::rotation(angle, centreX, centreY));

    const auto tip = juce::Point<float>(centreX, centreY - pointerLength)
                          .transformedBy(juce::AffineTransform::rotation(angle, centreX, centreY));
    g.setColour(accent);
    g.fillEllipse(juce::Rectangle<float>(5.5f, 5.5f).withCentre(tip));
}

void RigLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   juce::ComboBox& box) {
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)).reduced(1.0f);

    juce::ColourGradient bg(juce::Colour(RigDesign::kPanelColourAlt), bounds.getX(), bounds.getY(),
                             juce::Colour(RigDesign::kPanelColour), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

    const auto arrowZone = bounds.removeFromRight(28.0f);
    juce::Path arrow;
    arrow.startNewSubPath(arrowZone.getCentreX() - 5.0f, arrowZone.getCentreY() - 2.5f);
    arrow.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + 3.5f);
    arrow.lineTo(arrowZone.getCentreX() + 5.0f, arrowZone.getCentreY() - 2.5f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.strokePath(arrow, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void RigLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                       bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    const bool engaged = button.getToggleState();

    juce::ColourGradient bodyGradient(engaged ? juce::Colour(0xff3c3f46) : juce::Colour(0xff26282d),
                                       bounds.getX(), bounds.getY(),
                                       engaged ? juce::Colour(0xff26282d) : juce::Colour(0xff17181b),
                                       bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(bodyGradient);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff0d0e10));
    g.drawRoundedRectangle(bounds, 6.0f, 1.2f);

    const float ledDiameter = juce::jmin(14.0f, bounds.getHeight() * 0.35f);
    const auto ledBounds = juce::Rectangle<float>(ledDiameter, ledDiameter)
                                .withCentre({ bounds.getCentreX(), bounds.getY() + bounds.getHeight() * 0.32f });
    const juce::Colour ledColour = engaged ? juce::Colour(0xff5CFF7A) : juce::Colour(0xff5a1f1f);
    if (engaged) {
        g.setColour(ledColour.withAlpha(0.35f));
        g.fillEllipse(ledBounds.expanded(4.0f));
    }
    g.setColour(ledColour);
    g.fillEllipse(ledBounds);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(ledBounds, 1.0f);

    g.setColour(juce::Colour(RigDesign::kTextPrimary));
    g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    g.drawFittedText(button.getButtonText(),
                      bounds.withTrimmedTop(bounds.getHeight() * 0.5f).toNearestInt(),
                      juce::Justification::centred, 1);

    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}
