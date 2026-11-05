#pragma once

#include <optional>
#include "ActionKey.hpp"

namespace rtype::game {

/** @brief Component of an enemy: the attack and phase it is in, empty while
 * it is idle. World::spawn gives one to every body of side enemy, so a system
 * changes it from inside a forEach. */
struct CurrentAction {
  std::optional<ActionKey> key;
};

}  // namespace rtype::game
