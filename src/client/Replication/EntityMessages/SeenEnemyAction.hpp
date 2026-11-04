#pragma once

#include "EnemyAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

struct SeenEnemyAction {
  network::EnemyAction action{};
  engine::Duration age{};
};

}  // namespace rtype::client
