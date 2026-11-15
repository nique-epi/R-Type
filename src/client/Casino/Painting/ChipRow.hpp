#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client {

/** @brief The chips of a selector as a screen lays them: where the first one
 * lies at rest, its top-left corner, and the place in CHIP_VALUES of the one
 * chosen. */
struct ChipRow {
  sf::Vector2i firstChip;
  std::size_t chosen{};
};

}  // namespace rtype::client
