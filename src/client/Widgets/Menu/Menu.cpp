#include "Menu.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include <utility>
#include "BackgroundConstants.hpp"
#include "ButtonState.hpp"
#include "ChamferedBox.hpp"
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "Label.hpp"
#include "MenuContent.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

namespace {

/**
 * @returns The pixel of the interface picture under a point of the playfield;
 * a point left of or above the picture gives a negative pixel.
 */
sf::Vector2i interfacePixelAt(sf::Vector2f playfieldPoint) {
  return {
      static_cast<int>(std::floor(playfieldPoint.x / BACKGROUND_PIXEL_SIZE)),
      static_cast<int>(std::floor(playfieldPoint.y / BACKGROUND_PIXEL_SIZE))};
}

/** @returns Whether the key is one of the list. */
template <std::size_t KeyCount>
bool isOneOf(sf::Keyboard::Key key,
             const std::array<sf::Keyboard::Key, KeyCount>& keys) {
  return std::ranges::find(keys, key) != keys.end();
}

}  // namespace

Menu::Menu(MenuContent content) : content_(std::move(content)) {
  selectNeighbour(Direction::Next);
}

void Menu::handleEvent(const sf::Event& event,
                       const PlayfieldMapping& playfieldPointAt) {
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (const auto* released = event.getIf<sf::Event::KeyReleased>()) {
    keyReleased(released->code);
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    pointerMoved(interfacePixelAt(playfieldPointAt(moved->position)));
  } else if (const auto* clicked =
                 event.getIf<sf::Event::MouseButtonPressed>()) {
    if (clicked->button == sf::Mouse::Button::Left) {
      pointerPressed(interfacePixelAt(playfieldPointAt(clicked->position)));
    }
  } else if (const auto* unclicked =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    if (unclicked->button == sf::Mouse::Button::Left) {
      pointerReleased(interfacePixelAt(playfieldPointAt(unclicked->position)));
    }
  } else if (event.is<sf::Event::FocusLost>()) {
    focusLost();
  }
}

void Menu::advance(engine::Duration elapsed) {
  if (press_.has_value()) {
    press_->held += elapsed;
    activateIfDue();
  }
}

void Menu::paint(IInterfaceCanvas& canvas) const {
  if (content_.dimmed) {
    canvas.fill(INTERFACE_AREA, DIM_LAYER_COLOR);
  }
  if (content_.panel.has_value()) {
    paintChamferedBox(
        canvas, *content_.panel,
        {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR});
  }
  if (content_.rule.has_value()) {
    canvas.fill(*content_.rule, RULE_COLOR);
  }
  if (content_.picture.has_value()) {
    canvas.drawImage(content_.picture->assetId,
                     {{0, 0}, content_.picture->area.size},
                     content_.picture->area.position, INTERFACE_AREA);
  }
  for (const Label& label : content_.labels) {
    label.paint(canvas);
  }
  for (std::size_t button = 0; button < content_.buttons.size(); ++button) {
    content_.buttons[button].paint(canvas, stateOf(button));
  }
  if (selected_.has_value() &&
      !content_.buttons[*selected_].description().empty()) {
    canvas.write(content_.buttons[*selected_].description(),
                 content_.descriptionAnchor, PAGE_LINE_STYLE);
  }
}

void Menu::setEnabled(std::size_t button, bool enabled) {
  if (button >= content_.buttons.size()) {
    return;
  }
  content_.buttons[button].setEnabled(enabled);
  if (enabled) {
    if (!selected_.has_value()) {
      selected_ = button;
    }
    return;
  }
  if (press_.has_value() && press_->button == button) {
    press_.reset();
  }
  if (selected_ == button) {
    selectNeighbour(Direction::Next);
  }
}

void Menu::setMark(std::size_t button, CheckMark mark) {
  if (button < content_.buttons.size()) {
    content_.buttons[button].setMark(mark);
  }
}

std::optional<std::size_t> Menu::selectedButton() const { return selected_; }

std::optional<sf::IntRect> Menu::illustrationArea() const {
  return content_.illustration;
}

void Menu::keyPressed(const sf::Event::KeyPressed& key) {
  if (press_.has_value()) {
    return;
  }
  if (key.code == MENU_CONFIRM_KEY) {
    if (selected_.has_value()) {
      press_ = Press{.button = *selected_, .source = PressSource::Keyboard};
    }
  } else if (key.code == MENU_CANCEL_KEY) {
    cancelPressed_ = true;
  } else if (isOneOf(key.code, MENU_NEXT_KEYS) ||
             (key.code == MENU_TAB_KEY && !key.shift)) {
    selectNeighbour(Direction::Next);
  } else if (isOneOf(key.code, MENU_PREVIOUS_KEYS) ||
             (key.code == MENU_TAB_KEY && key.shift)) {
    selectNeighbour(Direction::Previous);
  }
}

void Menu::keyReleased(sf::Keyboard::Key key) {
  if (key == MENU_CONFIRM_KEY && press_.has_value() &&
      press_->source == PressSource::Keyboard) {
    press_->released = true;
    activateIfDue();
  } else if (key == MENU_CANCEL_KEY && cancelPressed_) {
    cancelPressed_ = false;
    if (!press_.has_value()) {
      activateCancel();
    }
  }
}

void Menu::pointerMoved(sf::Vector2i pixel) {
  if (press_.has_value()) {
    return;
  }
  const std::optional<std::size_t> button = enabledButtonAt(pixel);
  if (button.has_value()) {
    selected_ = button;
  }
}

void Menu::pointerPressed(sf::Vector2i pixel) {
  if (press_.has_value()) {
    return;
  }
  const std::optional<std::size_t> button = enabledButtonAt(pixel);
  if (button.has_value()) {
    selected_ = button;
    press_ = Press{.button = *button, .source = PressSource::Pointer};
  }
}

void Menu::pointerReleased(sf::Vector2i pixel) {
  if (!press_.has_value() || press_->source != PressSource::Pointer ||
      press_->released) {
    return;
  }
  if (enabledButtonAt(pixel) == press_->button) {
    press_->released = true;
    activateIfDue();
  } else {
    press_.reset();
  }
}

void Menu::focusLost() {
  cancelPressed_ = false;
  if (press_.has_value() && !press_->released) {
    press_.reset();
  }
}

void Menu::activateIfDue() {
  if (!press_.has_value() || !press_->released ||
      press_->held < PRESS_DURATION) {
    return;
  }
  const std::size_t button = press_->button;
  press_.reset();
  content_.buttons[button].activate();
}

void Menu::activateCancel() {
  if (!content_.cancelButton.has_value() ||
      !content_.buttons[*content_.cancelButton].isEnabled()) {
    return;
  }
  selected_ = content_.cancelButton;
  content_.buttons[*content_.cancelButton].activate();
}

void Menu::selectNeighbour(Direction direction) {
  const std::size_t count = content_.buttons.size();
  std::size_t button =
      selected_.value_or(direction == Direction::Next ? count - 1 : 0);
  for (std::size_t tried = 0; tried < count; ++tried) {
    button = direction == Direction::Next ? (button + 1) % count
                                          : (button + count - 1) % count;
    if (content_.buttons[button].isEnabled()) {
      selected_ = button;
      return;
    }
  }
  selected_.reset();
}

std::optional<std::size_t> Menu::enabledButtonAt(sf::Vector2i pixel) const {
  for (std::size_t button = 0; button < content_.buttons.size(); ++button) {
    if (content_.buttons[button].isEnabled() &&
        content_.buttons[button].contains(pixel)) {
      return button;
    }
  }
  return std::nullopt;
}

ButtonState Menu::stateOf(std::size_t button) const {
  if (!content_.buttons[button].isEnabled()) {
    return ButtonState::Disabled;
  }
  if (press_.has_value() && press_->button == button) {
    return ButtonState::Pressed;
  }
  if (selected_ == button) {
    return ButtonState::Selected;
  }
  return ButtonState::Idle;
}

}  // namespace rtype::client
