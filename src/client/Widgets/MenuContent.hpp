#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "Button.hpp"
#include "Label.hpp"

namespace rtype::client {

/** @brief An image of a menu, such as the logo: the texture it shows whole,
 * and where. */
struct MenuPicture {
  std::string assetId;
  sf::IntRect area;
};

/**
 * @brief Everything a menu shows, already placed in pixels of the interface
 * picture: built by MenuBuilder, never by a screen.
 */
struct MenuContent {
  /** @brief Whether the screens below are dimmed first. */
  bool dimmed = false;
  /** @brief The panel of a dialog. */
  std::optional<sf::IntRect> panel;
  /** @brief The logo of a page. */
  std::optional<MenuPicture> picture;
  /** @brief The rule under the header of a page. */
  std::optional<sf::IntRect> rule;
  std::vector<Label> labels;
  /** @brief The buttons, in the order they are selected. */
  std::vector<Button> buttons;
  /** @brief The button MENU_CANCEL_KEY activates. */
  std::optional<std::size_t> cancelButton;
  /** @brief Where the description of the selected button is written. */
  sf::Vector2i descriptionAnchor;
  /** @brief The area a screen paints its own picture in. */
  std::optional<sf::IntRect> illustration;
};

}  // namespace rtype::client
