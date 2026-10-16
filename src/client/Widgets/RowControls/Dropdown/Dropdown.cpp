#include "Dropdown.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BarState.hpp"
#include "ChamferedBox.hpp"
#include "CheckBox.hpp"
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControlConstants.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "ValueBinding.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

Dropdown::Dropdown(std::vector<std::string> choices,
                   std::function<bool(std::size_t)> available,
                   ValueBinding<std::size_t> value)
    : choices_(std::move(choices)),
      available_(std::move(available)),
      value_(std::move(value)) {}

int Dropdown::width() const { return DROPDOWN_BOX_SIZE.x; }

void Dropdown::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                     BarState row) const {
  const bool lit = open_ || row == BarState::Selected;
  SunkBox(area, open_ ? SunkBox::Sides::Lit : SunkBox::Sides::Shaded)
      .paint(canvas);
  canvas.write(choices_[value_.read()],
               {area.position.x + DROPDOWN_TEXT_LEFT,
                area.position.y + CONTROL_TEXT_TOP},
               {.font = TextFont::Regular,
                .color = lit ? SELECTED_BOXED_VALUE_COLOR : BOXED_VALUE_COLOR});
  paintPointer(canvas, area,
               lit ? SELECTED_FIELD_BORDER_COLOR : BOXED_VALUE_COLOR);
}

void Dropdown::paintOverlay(IInterfaceCanvas& canvas, sf::IntRect area) const {
  if (!open_) {
    return;
  }
  ChamferedBox(listArea(area),
               {.border = LIST_BORDER_COLOR, .inside = LIST_INSIDE_COLOR})
      .paint(canvas);
  for (std::size_t choice = 0; choice < choices_.size(); ++choice) {
    const sf::IntRect item = itemArea(choice, area);
    const bool takable = available_(choice);
    const bool highlighted = choice == highlight_;
    if (highlighted) {
      canvas.fill(item, LIST_HIGHLIGHT_COLOR);
    }
    sf::Color text = BOXED_VALUE_COLOR;
    if (!takable) {
      text = UNAVAILABLE_CHOICE_COLOR;
    } else if (highlighted) {
      text = LIST_HIGHLIGHT_TEXT_COLOR;
    }
    CheckMark mark =
        choice == value_.read() ? CheckMark::Checked : CheckMark::Unchecked;
    if (!takable) {
      mark = CheckMark::Unavailable;
    }
    CheckBox(item.position + LIST_CHECK_BOX_OFFSET, mark).paint(canvas, text);
    canvas.write(choices_[choice], item.position + LIST_TEXT_OFFSET,
                 {.font = TextFont::Regular, .color = text});
  }
}

bool Dropdown::isActivatable() const { return true; }

void Dropdown::activate() {
  open_ = true;
  highlight_ = value_.read();
  pointed_.reset();
}

bool Dropdown::holdsInput() const { return open_; }

bool Dropdown::keyPressed(const sf::Event::KeyPressed& key) {
  if (!open_) {
    return false;
  }
  std::optional<std::size_t> next;
  if (key.code == LIST_PREVIOUS_KEY && highlight_ > 0) {
    next = nextAvailable(highlight_ - 1, false);
  } else if (key.code == LIST_NEXT_KEY) {
    next = nextAvailable(highlight_ + 1, true);
  } else if (key.code == FIRST_VALUE_KEY) {
    next = nextAvailable(0, true);
  } else if (key.code == LAST_VALUE_KEY && !choices_.empty()) {
    next = nextAvailable(choices_.size() - 1, false);
  } else if (key.code == MENU_CONFIRM_KEY) {
    take(highlight_);
  } else if (key.code == MENU_CANCEL_KEY || key.code == MENU_TAB_KEY) {
    close();
  }
  if (next.has_value()) {
    highlight_ = *next;
  }
  return true;
}

bool Dropdown::pointerPressed(sf::Vector2i pixel, sf::IntRect area) {
  if (!open_) {
    if (!area.contains(pixel)) {
      return false;
    }
    activate();
    return true;
  }
  const std::optional<std::size_t> choice = choiceAt(pixel, area);
  if (choice.has_value()) {
    take(*choice);
  } else {
    close();
  }
  return true;
}

void Dropdown::pointerMoved(sf::Vector2i pixel, sf::IntRect area) {
  pointed_ = choiceAt(pixel, area);
  if (pointed_.has_value() && available_(*pointed_)) {
    highlight_ = *pointed_;
  }
}

void Dropdown::cancelInput() { close(); }

bool Dropdown::isOpen() const { return open_; }

std::optional<std::size_t> Dropdown::highlighted() const {
  return open_ ? std::optional<std::size_t>(highlight_) : std::nullopt;
}

std::optional<std::size_t> Dropdown::pointedChoice() const {
  return open_ ? pointed_ : std::nullopt;
}

sf::IntRect Dropdown::listArea(sf::IntRect area) const {
  const int height = (static_cast<int>(choices_.size()) * LIST_ITEM_HEIGHT) +
                     (SIDES_PER_AXIS * LIST_PADDING);
  return {{area.position.x, area.position.y + area.size.y},
          {area.size.x, height}};
}

sf::IntRect Dropdown::itemArea(std::size_t choice, sf::IntRect area) const {
  const sf::IntRect list = listArea(area);
  return {{list.position.x + LIST_PADDING,
           list.position.y + LIST_PADDING +
               (static_cast<int>(choice) * LIST_ITEM_HEIGHT)},
          {list.size.x - (SIDES_PER_AXIS * LIST_PADDING), LIST_ITEM_HEIGHT}};
}

std::optional<std::size_t> Dropdown::choiceAt(sf::Vector2i pixel,
                                              sf::IntRect area) const {
  for (std::size_t choice = 0; choice < choices_.size(); ++choice) {
    if (itemArea(choice, area).contains(pixel)) {
      return choice;
    }
  }
  return std::nullopt;
}

std::optional<std::size_t> Dropdown::nextAvailable(std::size_t from,
                                                   bool forward) const {
  for (std::size_t choice = from; choice < choices_.size();
       forward ? ++choice : --choice) {
    if (available_(choice)) {
      return choice;
    }
    if (!forward && choice == 0) {
      break;
    }
  }
  return std::nullopt;
}

void Dropdown::take(std::size_t choice) {
  if (!available_(choice)) {
    return;
  }
  value_.change(choice);
  close();
}

void Dropdown::close() {
  open_ = false;
  pointed_.reset();
}

void Dropdown::paintPointer(IInterfaceCanvas& canvas, sf::IntRect box,
                            sf::Color color) const {
  const int left = box.position.x + box.size.x - DROPDOWN_POINTER_RIGHT -
                   DROPDOWN_POINTER_SIZE.x;
  const int top = box.position.y + DROPDOWN_POINTER_TOP;
  for (int row = 0; row < DROPDOWN_POINTER_SIZE.y; ++row) {
    const int drawnRow = open_ ? DROPDOWN_POINTER_SIZE.y - 1 - row : row;
    canvas.fill({{left + row, top + drawnRow},
                 {DROPDOWN_POINTER_SIZE.x - (SIDES_PER_AXIS * row), 1}},
                color);
  }
}

}  // namespace rtype::client
