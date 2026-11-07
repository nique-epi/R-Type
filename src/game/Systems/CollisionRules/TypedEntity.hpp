#pragma once

#include "Entity.hpp"
#include "EntityType.hpp"

namespace rtype::game {

/** @brief One of the two entities of a collision, with its side. */
struct TypedEntity {
  engine::Entity entity{};
  EntityType type{};
};

}  // namespace rtype::game
