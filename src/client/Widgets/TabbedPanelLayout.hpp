#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client {

/**
 * @brief Where a tabbed panel lays its parts at one text size: its tabs, the
 * panel of rows under them, its rows and how many show, and the bars in a
 * row under the panel with the description beside them.
 */
struct TabbedPanelLayout {
  sf::Vector2i tabsTopLeft;
  int tabHeight{};
  sf::IntRect panel;
  sf::Vector2i firstRowTopLeft;
  sf::Vector2i rowSize;
  std::size_t rowCapacity{};
  int barsTop{};
  sf::Vector2i barSize;
};

}  // namespace rtype::client
