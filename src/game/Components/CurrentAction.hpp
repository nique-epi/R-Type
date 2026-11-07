#pragma once

#include <cstdint>
#include <optional>
#include "ActionKey.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief Component of an enemy: the attack and phase it is in, empty while
 * it is idle, and how long it has been so. World::spawn gives one to every
 * body of side enemy, so a system changes it from inside a forEach. */
struct CurrentAction {
  std::optional<ActionKey> key;
  /** @brief How long the enemy has been in key, or idle since its last action
   * ended. PilotSystem adds each tick before the behaviour steers;
   * startEnemyAction() and endEnemyAction() set it back to zero. */
  engine::Duration since{};
  /** @brief How many actions the enemy has ended since it appeared;
   * endEnemyAction() adds one. A behaviour that keeps no state takes its next
   * attack from it, as the Bydo Tyrant does. */
  std::uint32_t ended{0};
};

}  // namespace rtype::game
