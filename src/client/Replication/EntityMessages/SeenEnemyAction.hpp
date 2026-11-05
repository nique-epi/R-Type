#pragma once

#include <cstdint>
#include "EnemyAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief An (entity, attack, phase) an EnemyAction named, and how long ago. */
struct SeenEnemyAction {
  std::uint32_t entityId{};
  std::uint8_t attack{};
  network::EnemyActionPhase phase{network::EnemyActionPhase::Signal};
  engine::Duration age{};
};

}  // namespace rtype::client
