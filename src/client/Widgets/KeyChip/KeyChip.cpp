#include "KeyChip.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <optional>
#include <utility>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "KeyChipParts.hpp"
#include "PlayfieldMapping.hpp"
#include "SoundConstants.hpp"

namespace rtype::client {

KeyChip::KeyChip(KeyChipParts parts)
    : button_(std::move(parts.text), parts.area, std::move(parts.action),
              std::nullopt),
      key_(parts.key),
      sounds_(&parts.sounds.get()),
      restState_(parts.restState) {}

bool KeyChip::handleEvent(const sf::Event& event,
                          const PlayfieldMapping& playfieldPointAt) {
  if (event.is<sf::Event::FocusLost>()) {
    keyHeld_ = false;
    pointerHeld_ = false;
    return false;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>();
      pressed != nullptr && pressed->code == key_) {
    keyHeld_ = button_.isEnabled();
    return true;
  }
  if (const auto* released = event.getIf<sf::Event::KeyReleased>();
      released != nullptr && released->code == key_) {
    if (std::exchange(keyHeld_, false)) {
      activate();
    }
    return true;
  }
  return pointerEvent(event, playfieldPointAt);
}

void KeyChip::setEnabled(bool enabled) {
  button_.setEnabled(enabled);
  if (!enabled) {
    keyHeld_ = false;
    pointerHeld_ = false;
  }
}

void KeyChip::paint(IInterfaceCanvas& canvas) const {
  button_.paint(canvas, state());
}

bool KeyChip::pointerEvent(const sf::Event& event,
                           const PlayfieldMapping& playfieldPointAt) {
  const auto isOver = [&](sf::Vector2i position) {
    return button_.contains(interfacePixelAt(playfieldPointAt(position)));
  };
  if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    hovered_ = isOver(moved->position);
    return false;
  }
  if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>();
      pressed != nullptr && pressed->button == sf::Mouse::Button::Left &&
      isOver(pressed->position)) {
    pointerHeld_ = button_.isEnabled();
    return true;
  }
  const auto* released = event.getIf<sf::Event::MouseButtonReleased>();
  if (released == nullptr || released->button != sf::Mouse::Button::Left ||
      !pointerHeld_) {
    return false;
  }
  pointerHeld_ = false;
  if (isOver(released->position)) {
    activate();
  }
  return true;
}

void KeyChip::activate() {
  sounds_->play(CLICK_SOUND_ID);
  button_.activate();
}

BarState KeyChip::state() const {
  if (!button_.isEnabled()) {
    return BarState::Disabled;
  }
  if (keyHeld_ || pointerHeld_) {
    return BarState::Pressed;
  }
  return hovered_ ? BarState::Selected : restState_;
}

}  // namespace rtype::client
