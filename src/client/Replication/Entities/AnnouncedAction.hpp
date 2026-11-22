#pragma once

#include "ActionKey.hpp"

namespace rtype::client {

/** @brief An action an enemy entered, and the server tick it was announced
 * at, as the client estimated it when the EnemyAction arrived. */
struct AnnouncedAction {
  game::ActionKey key{};
  double serverTick{0.0};
};

}  // namespace rtype::client
