#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <vector>
#include "ItemCell.hpp"

namespace rtype::client {

/** @brief A row of an ItemList: a cell per column, left to right, and the color
 * filled behind it, none letting what lies under show. */
struct ItemRow {
  std::vector<ItemCell> cells;
  std::optional<sf::Color> highlight = std::nullopt;
};

}  // namespace rtype::client
