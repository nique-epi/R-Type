#pragma once

#include <cstdint>
#include "EnemyPhase.hpp"

namespace rtype::game {

/** @brief One attack of an enemy and the phase it is in. `attack` is a number
 * the sheet of each type of enemy gives its own attacks. */
struct ActionKey {
  std::uint8_t attack{0};
  EnemyPhase phase{EnemyPhase::Signal};

  bool operator==(const ActionKey&) const = default;
};

}  // namespace rtype::game
