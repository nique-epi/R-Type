#include "KeySlot.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <utility>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyCap.hpp"
#include "RowControlConstants.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

KeySlot::KeySlot(Wiring wiring) : wiring_(std::move(wiring)) {}

int KeySlot::width() const {
  if (listening_) {
    return regularTextWidth(KEY_LISTENING_TEXT) +
           (SIDES_PER_AXIS * KEY_LISTENING_PADDING);
  }
  const int keyCapWidth =
      KeyCap(wiring_.name(wiring_.key.read()), KeyCap::Edge::Plain).width();
  if (!wiring_.conflicts()) {
    return keyCapWidth;
  }
  return regularTextWidth(KEY_CONFLICT_FLAG) + CONTROL_GAP + keyCapWidth;
}

void KeySlot::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                    BarState row) const {
  const sf::Vector2i textAnchor(area.position.x,
                                area.position.y + CONTROL_TEXT_TOP);
  if (listening_) {
    SunkBox(area, SunkBox::Sides::Lit).paint(canvas);
    canvas.write(KEY_LISTENING_TEXT,
                 textAnchor + sf::Vector2i(KEY_LISTENING_PADDING, 0),
                 {.font = TextFont::Regular, .color = AWAITED_VALUE_COLOR});
    return;
  }
  const bool selected = row == BarState::Selected;
  KeyCap::Edge edge = selected ? KeyCap::Edge::Selected : KeyCap::Edge::Plain;
  sf::Vector2i keyCapTopLeft = area.position;
  if (wiring_.conflicts()) {
    edge = KeyCap::Edge::Conflict;
    canvas.write(KEY_CONFLICT_FLAG, textAnchor,
                 {.font = TextFont::Regular,
                  .color = selected ? barTextColor(row) : CONFLICT_FLAG_COLOR});
    keyCapTopLeft.x += regularTextWidth(KEY_CONFLICT_FLAG) + CONTROL_GAP;
  }
  KeyCap(wiring_.name(wiring_.key.read()), edge).paint(canvas, keyCapTopLeft);
}

bool KeySlot::isActivatable() const { return true; }

void KeySlot::activate() {
  listening_ = true;
  lastKeyRefused_ = false;
}

bool KeySlot::holdsInput() const {
  return listening_ || takenKeyHeld_.has_value();
}

bool KeySlot::keyPressed(const sf::Event::KeyPressed& key) {
  if (!listening_) {
    return takenKeyHeld_.has_value();
  }
  lastKeyRefused_ = false;
  if (key.code == MENU_CANCEL_KEY) {
    listening_ = false;
  } else if (!wiring_.accepts(key.scancode)) {
    lastKeyRefused_ = true;
  } else {
    listening_ = false;
    takenKeyHeld_ = key.scancode;
    wiring_.key.change(key.scancode);
  }
  return true;
}

void KeySlot::keyReleased(const sf::Event::KeyReleased& key) {
  if (takenKeyHeld_ == key.scancode) {
    takenKeyHeld_.reset();
  }
}

bool KeySlot::pointerPressed([[maybe_unused]] sf::Vector2i pixel,
                             [[maybe_unused]] sf::IntRect area) {
  if (!listening_) {
    return false;
  }
  listening_ = false;
  return true;
}

void KeySlot::cancelInput() {
  listening_ = false;
  takenKeyHeld_.reset();
}

bool KeySlot::isListening() const { return listening_; }

bool KeySlot::lastKeyRefused() const { return lastKeyRefused_; }

}  // namespace rtype::client
