#pragma once

#include "Bounds.hpp"
#include "Entity.hpp"

namespace rtype::game {

/** @brief An enemy as it was at one tick: the entity and its box there. */
struct PastEnemy {
  engine::Entity entity{};
  engine::Bounds bounds{};
};

}  // namespace rtype::game
