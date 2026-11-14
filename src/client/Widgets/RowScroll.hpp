#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include "RowWindow.hpp"

namespace rtype::client {

/**
 * @brief The rows of a panel that holds more rows than it shows: which of
 * them show, the order of the first row among the buttons of its menu, how
 * far apart two rows lie, and the area the shown rows cover.
 *
 * Each row keeps the place it would have unscrolled; the panel paints the
 * shown ones moved up by the rows above them, and reads a pointer over the
 * area back to that place.
 */
struct RowScroll {
  RowWindow rows;
  std::size_t firstButton{};
  int pitch{};
  sf::IntRect viewport;

  bool operator==(const RowScroll& other) const = default;
};

}  // namespace rtype::client
