#include "ChainStripComponent.h"
#include "PedalCard.h"
#include "RigDesign.h"
#include <algorithm>

class ChainStripComponent::Content : public juce::Component {
public:
    Content(audio::GuitarRigEngine& engineRef, ChainStripComponent& ownerRef)
        : engine(engineRef), owner(ownerRef) {
        addButton.setButtonText("+  Add Effect");
        addButton.onClick = [this] { if (owner.onAddRequested) owner.onAddRequested(); };
        addAndMakeVisible(addButton);
        rebuild();
    }

    void rebuild() {
        cards.clear();
        auto& chain = engine.getChain();
        for (size_t i = 0; i < chain.size(); ++i) {
            auto* fx = chain.getEffect(i);
            auto card = std::make_unique<PedalCard>(fx->name(), fx->category(),
                                                      RigDesign::colourForCategory(fx->category()));
            card->setEngaged(!fx->isBypassed());
            PedalCard* cardPtr = card.get();

            card->onSelect = [this, cardPtr] {
                const int idx = indexOfCard(cardPtr);
                if (idx >= 0) owner.setSelectedIndex(idx);
            };
            card->onBypassToggle = [this, cardPtr](bool engaged) {
                const int idx = indexOfCard(cardPtr);
                if (idx >= 0)
                    if (auto* effect = engine.getChain().getEffect(static_cast<size_t>(idx)))
                        effect->setBypassed(!engaged);
            };
            card->onRemove = [this, cardPtr] {
                const int idx = indexOfCard(cardPtr);
                if (idx < 0) return;
                engine.removeModule(static_cast<size_t>(idx));
                if (owner.onChainChanged) owner.onChainChanged();
            };
            card->onDragStart = [this, cardPtr](const juce::MouseEvent&) { draggedCard = cardPtr; };
            card->onDragMove = [this, cardPtr](const juce::MouseEvent& e) { handleDragMove(cardPtr, e); };
            card->onDragEnd = [this](const juce::MouseEvent&) { draggedCard = nullptr; layoutCards(); };

            addAndMakeVisible(*card);
            cards.push_back(std::move(card));
        }
        layoutCards();
    }

    int indexOfCard(PedalCard* target) const {
        for (size_t i = 0; i < cards.size(); ++i)
            if (cards[i].get() == target) return static_cast<int>(i);
        return -1;
    }

    void setSelectedVisual(int index) {
        for (size_t i = 0; i < cards.size(); ++i)
            cards[i]->setSelected(static_cast<int>(i) == index);
    }

    size_t numCards() const { return cards.size(); }

    void handleDragMove(PedalCard* card, const juce::MouseEvent& e) {
        const int currentIndex = indexOfCard(card);
        if (currentIndex < 0) return;

        const auto posInContent = e.getEventRelativeTo(this).position;
        int newX = juce::roundToInt(posInContent.x - kCardWidth * 0.5f);
        newX = juce::jlimit(kMargin, juce::jmax(kMargin, getWidth() - kCardWidth - kMargin), newX);
        card->setTopLeftPosition(newX, kMargin);

        const int slot = juce::jlimit(0, static_cast<int>(cards.size()) - 1,
                                       (newX - kMargin) / (kCardWidth + kGap));
        if (slot != currentIndex) {
            auto moved = std::move(cards[static_cast<size_t>(currentIndex)]);
            cards.erase(cards.begin() + currentIndex);
            cards.insert(cards.begin() + slot, std::move(moved));
            engine.moveModule(static_cast<size_t>(currentIndex), static_cast<size_t>(slot));
            owner.selectedIndex = slot;
            setSelectedVisual(slot);
            layoutCards(card);
            if (owner.onSelectionChanged) owner.onSelectionChanged(slot);
        }
    }

    void layoutCards(PedalCard* exceptBeingDragged = nullptr) {
        int x = kMargin;
        for (auto& c : cards) {
            if (c.get() != exceptBeingDragged)
                c->setBounds(x, kMargin, kCardWidth, kCardHeight);
            x += kCardWidth + kGap;
        }
        addButton.setBounds(x, kMargin + (kCardHeight - 36) / 2, 150, 36);
        x += 150 + kMargin;
        setSize(juce::jmax(x, 200), kCardHeight + kMargin * 2);
    }

    void resized() override { layoutCards(draggedCard); }

    static constexpr int kCardWidth = 132;
    static constexpr int kCardHeight = 128;
    static constexpr int kGap = 10;
    static constexpr int kMargin = 10;

    audio::GuitarRigEngine& engine;
    ChainStripComponent& owner;
    std::vector<std::unique_ptr<PedalCard>> cards;
    juce::TextButton addButton;
    PedalCard* draggedCard = nullptr;
};

ChainStripComponent::ChainStripComponent(audio::GuitarRigEngine& engineToUse) : engine(engineToUse) {
    content = std::make_unique<Content>(engine, *this);
    viewport.setViewedComponent(content.get(), false);
    viewport.setScrollBarsShown(false, true);
    addAndMakeVisible(viewport);
    setSelectedIndex(content->numCards() > 0 ? 0 : -1);
}

ChainStripComponent::~ChainStripComponent() = default;

void ChainStripComponent::refresh() {
    content->rebuild();
    const int clamped = content->numCards() == 0
                             ? -1
                             : juce::jlimit(0, static_cast<int>(content->numCards()) - 1, selectedIndex);
    setSelectedIndex(clamped);
}

void ChainStripComponent::setSelectedIndex(int index) {
    selectedIndex = index;
    content->setSelectedVisual(index);
    if (onSelectionChanged) onSelectionChanged(index);
}

void ChainStripComponent::resized() {
    viewport.setBounds(getLocalBounds());
}

void ChainStripComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colour(RigDesign::kBackgroundBottom));
    g.fillRect(getLocalBounds());
}
