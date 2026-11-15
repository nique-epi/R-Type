#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Component of an enemy no hit takes anything from while it is in any
 * phase of its attack `attack`: the Bydo Tyrant, during its arrival. Out of
 * that attack, a hit costs what it costs any enemy.
 */
struct Armored {
  std::uint8_t attack{0};
};

}  // namespace rtype::game
