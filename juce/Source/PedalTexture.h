#pragma once

#include <JuceHeader.h>

// Original, procedurally-generated vintage-stompbox art: a tileable
// hammertone-paint speckle texture and a corner-screw drawing helper. No
// photos or scanned artwork are used anywhere - everything here is drawn
// from primitives so it stays crisp at any size and works in any accent
// colour.
namespace PedalTexture {

inline const juce::Image& hammertoneTexture() {
    static const juce::Image texture = [] {
        juce::Image img(juce::Image::ARGB, 96, 96, true);
        juce::Graphics g(img);
        juce::Random rng(90210);
        for (int i = 0; i < 260; ++i) {
            const float x = rng.nextFloat() * 96.0f;
            const float y = rng.nextFloat() * 96.0f;
            const float r = 0.5f + rng.nextFloat() * 2.1f;
            const bool light = rng.nextFloat() > 0.42f;
            const float alpha = 0.05f + rng.nextFloat() * 0.16f;
            g.setColour((light ? juce::Colours::white : juce::Colours::black).withAlpha(alpha));
            g.fillEllipse(x, y, r, r * (0.6f + rng.nextFloat() * 0.8f));
        }
        return img;
    }();
    return texture;
}

// Paints a hammertone-textured enclosure panel: base gradient in the given
// accent colour, then the speckle texture tiled on top, then a chrome bevel.
inline void paintEnclosure(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour accent,
                            float cornerSize, bool engaged) {
    const float desat = engaged ? 1.0f : 0.35f;
    const juce::Colour top = accent.withMultipliedSaturation(desat).darker(0.15f);
    const juce::Colour bottom = accent.withMultipliedSaturation(desat).darker(0.55f);

    juce::ColourGradient body(top, bounds.getX(), bounds.getY(), bottom, bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(body);
    g.fillRoundedRectangle(bounds, cornerSize);

    g.saveState();
    juce::Path clip;
    clip.addRoundedRectangle(bounds, cornerSize);
    g.reduceClipRegion(clip);
    g.setTiledImageFill(hammertoneTexture(), static_cast<int>(bounds.getX()), static_cast<int>(bounds.getY()), 1.0f);
    g.fillRect(bounds);
    g.restoreState();

    // Chrome bevel: light along the top edge, dark along the bottom.
    g.setColour(juce::Colours::white.withAlpha(0.22f));
    g.drawRoundedRectangle(bounds.reduced(0.75f), cornerSize, 1.0f);
    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.drawRoundedRectangle(bounds.reduced(1.6f), cornerSize * 0.9f, 1.0f);
}

inline void drawScrew(juce::Graphics& g, juce::Point<float> centre, float radius) {
    juce::ColourGradient metal(juce::Colour(0xffe4e6ea), centre.x - radius, centre.y - radius,
                                juce::Colour(0xff6b6e75), centre.x + radius, centre.y + radius, false);
    g.setGradientFill(metal);
    g.fillEllipse(juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre));
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.drawEllipse(juce::Rectangle<float>(radius * 2.0f, radius * 2.0f).withCentre(centre), 0.6f);

    // Screw slot, tilted slightly per-instance via the centre's position so a
    // row of screws doesn't look mechanically identical.
    const float angle = std::fmod(centre.x * 0.7f + centre.y * 1.3f, juce::MathConstants<float>::pi);
    juce::Line<float> slot(centre.getPointOnCircumference(radius * 0.75f, angle),
                            centre.getPointOnCircumference(radius * 0.75f, angle + juce::MathConstants<float>::pi));
    g.setColour(juce::Colours::black.withAlpha(0.65f));
    g.drawLine(slot, radius * 0.35f);
}

} // namespace PedalTexture
