#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Component of every entity made from a sheet: the number of its sheet
 * among the sheets the server loaded, from 1, as EntitySheet::number gives it.
 * The server will name the type of the entity on the wire by it.
 */
struct EntityTypeNumber {
  std::uint8_t number{0};
};

}  // namespace rtype::game
