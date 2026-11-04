#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <span>
#include "ItemListColumn.hpp"

namespace rtype::client {

/** @brief Where an ItemList lies: the top-left corner of its first row, how
 * wide a row is, how far apart two rows are, how many rows it shows at most,
 * and its columns, which must outlive it: a constant array does. */
struct ItemListLayout {
  sf::Vector2i topLeft;
  int width{};
  int rowPitch{};
  std::size_t rowCapacity{};
  std::span<const ItemListColumn> columns;
};

}  // namespace rtype::client
