#include "WindowSizeButtons.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include "ClientException.hpp"
#include "PixelSize.hpp"
#include "WindowColors.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

WindowSizeButtons::WindowSizeButtons() {
  if (!font_.openFromFile(WINDOW_SIZE_LABEL_FONT_FILE)) {
    throw FontNotLoadedException(WINDOW_SIZE_LABEL_FONT_FILE);
  }
  buttons_.reserve(WINDOW_SIZES.size());
  labels_.reserve(WINDOW_SIZES.size());
  float left = WINDOW_SIZE_BUTTONS_MARGIN;
  for (const PixelSize size : WINDOW_SIZES) {
    sf::RectangleShape& button = buttons_.emplace_back(
        sf::Vector2f(WINDOW_SIZE_BUTTON_WIDTH, WINDOW_SIZE_BUTTON_HEIGHT));
    button.setPosition({left, WINDOW_SIZE_BUTTONS_MARGIN});

    sf::Text& label = labels_.emplace_back(font_,
                                           std::to_string(size.width) +
                                               WINDOW_SIZE_SEPARATOR +
                                               std::to_string(size.height),
                                           WINDOW_SIZE_LABEL_CHARACTER_SIZE);
    label.setOrigin(label.getLocalBounds().getCenter());
    label.setPosition(button.getGlobalBounds().getCenter());

    left += WINDOW_SIZE_BUTTON_WIDTH + WINDOW_SIZE_BUTTONS_GAP;
  }
}

WindowSizeButtons::Colors WindowSizeButtons::colorsOf(
    std::size_t index, const WindowSizeSelection& selection) {
  if (index == selection.selectedIndex()) {
    return {.button = SELECTED_WINDOW_SIZE_COLOR,
            .label = SELECTED_WINDOW_SIZE_LABEL_COLOR};
  }
  if (selection.isAvailable(index)) {
    return {.button = AVAILABLE_WINDOW_SIZE_COLOR,
            .label = AVAILABLE_WINDOW_SIZE_LABEL_COLOR};
  }
  return {.button = UNAVAILABLE_WINDOW_SIZE_COLOR,
          .label = UNAVAILABLE_WINDOW_SIZE_LABEL_COLOR};
}

void WindowSizeButtons::showState(const WindowSizeSelection& selection) {
  for (std::size_t index = 0; index < buttons_.size(); ++index) {
    const Colors colors = colorsOf(index, selection);
    buttons_[index].setFillColor(colors.button);
    labels_[index].setFillColor(colors.label);
  }
}

std::optional<std::size_t> WindowSizeButtons::indexAt(
    sf::Vector2f point) const {
  for (std::size_t index = 0; index < buttons_.size(); ++index) {
    if (buttons_[index].getGlobalBounds().contains(point)) {
      return index;
    }
  }
  return std::nullopt;
}

void WindowSizeButtons::draw(sf::RenderTarget& target) const {
  for (std::size_t index = 0; index < buttons_.size(); ++index) {
    target.draw(buttons_[index]);
    target.draw(labels_[index]);
  }
}

}  // namespace rtype::client
