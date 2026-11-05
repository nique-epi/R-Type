#pragma once

#include "ActionKey.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An enemy and the action it enters. */
struct EnemyActionStart {
  engine::Entity enemy{};
  ActionKey action;
};

}  // namespace rtype::game
