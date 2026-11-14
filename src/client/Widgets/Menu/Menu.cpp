#include "Menu.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BarState.hpp"
#include "Button.hpp"
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "InterfacePixel.hpp"
#include "Label.hpp"
#include "PaintedButton.hpp"
#include "PlayfieldMapping.hpp"
#include "RowControl.hpp"
#include "RowScroll.hpp"
#include "RowScrolling.hpp"
#include "RowWindow.hpp"
#include "ShiftedCanvas.hpp"
#include "SoundConstants.hpp"
#include "TextField.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WrappedLines.hpp"

namespace rtype::client {

namespace {

/** @brief Plays the click of a bar or a tab, when the menu has a sound
 * player. */
void playClick(ISoundPlayer* sounds) {
  if (sounds != nullptr) {
    sounds->play(CLICK_SOUND_ID);
  }
}

/** @returns Whether activating the button plays the click: a bar does, a
 * field or a row does not. */
bool clicksWhenActivated(const Button& button) {
  return button.control() == nullptr && button.field() == nullptr;
}

/** @returns The press of the left button the event is, or nullptr. */
const sf::Event::MouseButtonPressed* leftClickOf(const sf::Event& event) {
  const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>();
  return clicked != nullptr && clicked->button == sf::Mouse::Button::Left
             ? clicked
             : nullptr;
}

/** @returns The release of the left button the event is, or nullptr. */
const sf::Event::MouseButtonReleased* leftReleaseOf(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::MouseButtonReleased>();
  return released != nullptr && released->button == sf::Mouse::Button::Left
             ? released
             : nullptr;
}

/** @returns How many rows a turn of the wheel scrolls: one down for each notch
 * turned toward the player, one up for each notch away. */
int wheelStepOf(const sf::Event::MouseWheelScrolled& wheel) {
  return wheel.delta < 0.0F ? 1 : -1;
}

/** @returns Whether the key is one of the list. */
bool isOneOf(sf::Keyboard::Key key, std::span<const sf::Keyboard::Key> keys) {
  return std::ranges::find(keys, key) != keys.end();
}

/** @returns The canvas a button of @p content paints on: @p canvas, or the
 * same moved up by the rows above the shown ones for a row of a panel that
 * scrolls. */
ShiftedCanvas canvasOfButton(const Menu::Content& content,
                             IInterfaceCanvas& canvas) {
  return {canvas, content.rowScroll.has_value()
                      ? rowOffsetOf(*content.rowScroll)
                      : sf::Vector2i()};
}

/** @returns Whether a button of @p content lies in its scrolling rows. */
bool scrollsWithTheRows(const Menu::Content& content, std::size_t button) {
  return content.rowScroll.has_value() &&
         button >= content.rowScroll->firstButton &&
         button <
             content.rowScroll->firstButton + content.rowScroll->rows.count;
}

/** @brief Paints a button of @p content where it shows: a row of a panel that
 * scrolls moves with the rows, and a row out of view is not painted. */
void paintButton(const Menu::Content& content, PaintedButton painted) {
  const Button& button = content.buttons[painted.order];
  if (!scrollsWithTheRows(content, painted.order)) {
    button.paint(painted.canvas.get(), painted.state);
    return;
  }
  if (!content.rowScroll.has_value() ||
      isHiddenRow(*content.rowScroll, painted.order)) {
    return;
  }
  ShiftedCanvas shifted = canvasOfButton(content, painted.canvas.get());
  button.paint(shifted, painted.state);
}

/** @brief Paints, in the padding of a panel that scrolls, a mark over its
 * rows when rows are hidden above, and one under them when rows are hidden
 * below. */
void paintScrollMarks(const Menu::Content& content, IInterfaceCanvas& canvas) {
  if (!content.rowScroll.has_value()) {
    return;
  }
  const RowScroll& scroll = *content.rowScroll;
  const sf::IntRect area = scroll.viewport;
  const int middle = area.position.x + area.size.x - SCROLL_MARK_INSET;
  if (scroll.rows.first > 0) {
    paintScrollArrow(canvas, {middle, area.position.y - SCROLL_MARK_ROWS}, 1);
  }
  if (scroll.rows.first < lastFirstRowOf(scroll.rows)) {
    paintScrollArrow(
        canvas, {middle, area.position.y + area.size.y + SCROLL_MARK_ROWS}, -1);
  }
}

/** @brief Writes the description of the selected button where @p content
 * asks, on as many lines as its width needs, each word whole. */
void paintDescription(const Menu::Content& content, IInterfaceCanvas& canvas,
                      std::string_view description) {
  if (!content.descriptionAnchor.has_value() || description.empty()) {
    return;
  }
  writeWrappedLines(canvas, description,
                    {.anchor = *content.descriptionAnchor,
                     .width = content.descriptionWidth,
                     .style = content.descriptionStyle});
}

/** @brief Draws the picture of @p content at its place, when it has one. */
void paintPicture(const Menu::Content& content, IInterfaceCanvas& canvas) {
  if (!content.picture.has_value()) {
    return;
  }
  canvas.drawImage(
      TextureArea{.assetId = content.picture->assetId,
                  .rectangle = {{0, 0}, content.picture->area.size}},
      ImagePlacement{.topLeft = content.picture->area.position},
      INTERFACE_AREA);
}

/** @brief Writes the status line where @p content asks, on as many lines as
 * its width needs when it has one, each line in the style of the status. */
void paintStatus(const Menu::Content& content, IInterfaceCanvas& canvas,
                 std::string_view status) {
  if (!content.statusAnchor.has_value() || status.empty()) {
    return;
  }
  writeWrappedLines(canvas, status,
                    {.anchor = *content.statusAnchor,
                     .width = content.statusWidth,
                     .style = content.statusStyle});
}

/** @brief Paints the marks of the rows hidden by a scroll, then what the
 * buttons of @p content lay over everything, such as
 * an open list, each moved with its row. */
void paintOverlays(const Menu::Content& content, IInterfaceCanvas& canvas) {
  paintScrollMarks(content, canvas);
  ShiftedCanvas shifted = canvasOfButton(content, canvas);
  for (std::size_t order = 0; order < content.buttons.size(); ++order) {
    if (scrollsWithTheRows(content, order)) {
      content.buttons[order].paintOverlay(shifted);
    } else {
      content.buttons[order].paintOverlay(canvas);
    }
  }
}

}  // namespace

Menu::Menu(Content content, ISoundPlayer* sounds)
    : content_(std::move(content)), sounds_(sounds) {
  selectNeighbour(Direction::Next);
}

void Menu::handleEvent(const sf::Event& event,
                       const PlayfieldMapping& playfieldPointAt) {
  const PlayfieldMapping mapping =
      scrolledMappingOf(content_.rowScroll, playfieldPointAt);
  const std::optional<std::size_t> holder = rowHoldingInput();
  if (holder.has_value()) {
    routeToControl(content_.buttons[*holder], event, mapping);
    if (event.is<sf::Event::FocusLost>()) {
      focusLost();
    }
    return;
  }
  TextField* field = selectedField();
  if (field != nullptr && !press_.has_value() && field->handleEvent(event)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (const auto* released = event.getIf<sf::Event::KeyReleased>()) {
    keyReleased(released->code);
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    pointerMoved(interfacePixelAt(mapping(moved->position)));
  } else if (const auto* clicked = leftClickOf(event)) {
    pointerPressed(interfacePixelAt(mapping(clicked->position)));
  } else if (const auto* unclicked = leftReleaseOf(event)) {
    pointerReleased(interfacePixelAt(mapping(unclicked->position)));
  } else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>();
             wheel != nullptr && content_.rowScroll.has_value()) {
    content_.rowScroll =
        scrollMovedBy(*content_.rowScroll, wheelStepOf(*wheel));
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
    CutCornerBox(*content_.panel,
                 {.border = PANEL_BORDER_COLOR, .inside = PANEL_INSIDE_COLOR})
        .paint(canvas);
  }
  if (content_.tabs.has_value()) {
    content_.tabs->paint(canvas);
  }
  if (content_.rule.has_value()) {
    canvas.fill(*content_.rule, RULE_COLOR);
  }
  paintPicture(content_, canvas);
  for (const Label& label : content_.labels) {
    label.paint(canvas);
  }
  for (std::size_t button = 0; button < content_.buttons.size(); ++button) {
    paintButton(content_, PaintedButton{.order = button,
                                        .state = stateOf(button),
                                        .canvas = canvas});
  }
  if (selected_.has_value()) {
    paintDescription(content_, canvas,
                     content_.buttons[*selected_].description());
  }
  paintStatus(content_, canvas, status_);
  paintOverlays(content_, canvas);
}

void Menu::setStatus(std::string text) { status_ = std::move(text); }

std::string_view Menu::fieldText(std::size_t button) const {
  if (button >= content_.buttons.size()) {
    return {};
  }
  const TextField* field = content_.buttons[button].field();
  return field == nullptr ? std::string_view()
                          : std::string_view(field->text());
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
  RowControl* const control = content_.buttons[button].control();
  if (control != nullptr) {
    control->cancelInput();
  }
  if (press_.has_value() && press_->button == button) {
    press_.reset();
  }
  if (selected_ == button) {
    selectNeighbour(Direction::Next);
  }
}

void Menu::setDescription(std::size_t button, std::string description) {
  if (button < content_.buttons.size()) {
    content_.buttons[button].setDescription(std::move(description));
  }
}

void Menu::setLabel(std::size_t button, std::string label) {
  if (button < content_.buttons.size()) {
    content_.buttons[button].setLabel(std::move(label));
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
  if (content_.tabs.has_value() && key.scancode == PREVIOUS_TAB_KEY) {
    openTab(content_.tabs->previousTab());
    return;
  }
  if (content_.tabs.has_value() && key.scancode == NEXT_TAB_KEY) {
    openTab(content_.tabs->nextTab());
    return;
  }
  if (key.code != MENU_CONFIRM_KEY && key.code != MENU_CANCEL_KEY &&
      offerToSelectedControl(key)) {
    return;
  }
  if (key.code == MENU_CONFIRM_KEY) {
    if (selected_.has_value() && content_.buttons[*selected_].isActivatable()) {
      press_ = Press{.button = *selected_, .source = PressSource::Keyboard};
    }
  } else if (key.code == MENU_CANCEL_KEY) {
    cancelPressed_ = true;
  } else if (isOneOf(key.code, content_.nextKeys) ||
             (key.code == MENU_TAB_KEY && !key.shift)) {
    selectNeighbour(Direction::Next);
  } else if (isOneOf(key.code, content_.previousKeys) ||
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
  if (press_.has_value() || !content_.selectsOnHover) {
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
  if (content_.tabs.has_value()) {
    const std::optional<std::size_t> tab = content_.tabs->tabOpenedAt(pixel);
    if (tab.has_value()) {
      openTab(*tab);
      return;
    }
  }
  const std::optional<std::size_t> button = enabledButtonAt(pixel);
  if (!button.has_value()) {
    return;
  }
  selected_ = button;
  Button& pressed = content_.buttons[*button];
  RowControl* const control = pressed.control();
  if (control != nullptr &&
      control->pointerPressed(pixel, pressed.controlArea())) {
    return;
  }
  if (pressed.field() == nullptr && pressed.isActivatable()) {
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
  if (clicksWhenActivated(content_.buttons[button])) {
    playClick(sounds_);
  }
  content_.buttons[button].activate();
}

void Menu::activateCancel() {
  if (!content_.cancelButton.has_value() ||
      !content_.buttons[*content_.cancelButton].isEnabled()) {
    return;
  }
  selected_ = content_.cancelButton;
  if (clicksWhenActivated(content_.buttons[*content_.cancelButton])) {
    playClick(sounds_);
  }
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
      if (content_.rowScroll.has_value()) {
        content_.rowScroll = scrollShowing(*content_.rowScroll, button);
      }
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

void Menu::routeToControl(Button& row, const sf::Event& event,
                          const PlayfieldMapping& playfieldPointAt) {
  RowControl* const control = row.control();
  if (control == nullptr) {
    return;
  }
  const sf::IntRect area = row.controlArea();
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    control->keyPressed(*pressed);
  } else if (const auto* released = event.getIf<sf::Event::KeyReleased>()) {
    control->keyReleased(*released);
  } else if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
    control->pointerMoved(interfacePixelAt(playfieldPointAt(moved->position)),
                          area);
  } else if (const auto* clicked =
                 event.getIf<sf::Event::MouseButtonPressed>()) {
    if (clicked->button == sf::Mouse::Button::Left) {
      control->pointerPressed(
          interfacePixelAt(playfieldPointAt(clicked->position)), area);
    }
  } else if (const auto* unclicked =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    if (unclicked->button == sf::Mouse::Button::Left) {
      control->pointerReleased(
          interfacePixelAt(playfieldPointAt(unclicked->position)), area);
    }
  } else if (event.is<sf::Event::FocusLost>()) {
    control->cancelInput();
  }
}

bool Menu::offerToSelectedControl(const sf::Event::KeyPressed& key) {
  if (!selected_.has_value()) {
    return false;
  }
  RowControl* const control = content_.buttons[*selected_].control();
  return control != nullptr && control->keyPressed(key);
}

void Menu::openTab(std::size_t tab) const {
  if (content_.openTab != nullptr) {
    playClick(sounds_);
    content_.openTab(tab);
  }
}

std::optional<std::size_t> Menu::rowHoldingInput() const {
  if (press_.has_value() || !selected_.has_value()) {
    return std::nullopt;
  }
  const RowControl* const control = content_.buttons[*selected_].control();
  if (control == nullptr || !control->holdsInput()) {
    return std::nullopt;
  }
  return selected_;
}

BarState Menu::stateOf(std::size_t button) const {
  if (!content_.buttons[button].isEnabled()) {
    return BarState::Disabled;
  }
  if (press_.has_value() && press_->button == button) {
    return BarState::Pressed;
  }
  if (selected_ == button) {
    return BarState::Selected;
  }
  return BarState::Idle;
}

TextField* Menu::selectedField() {
  return selected_.has_value() ? content_.buttons[*selected_].field() : nullptr;
}

}  // namespace rtype::client
