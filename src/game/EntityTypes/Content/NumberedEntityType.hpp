#pragma once

#include <string>
#include "CollisionBox.hpp"
#include "EntityType.hpp"

namespace rtype::game {

/**
 * @brief What a client knows of an entity type it draws: the name of its
 * sheet, its side and its box, which the client tests its own missiles
 * against. Its number is its place in the list
 * numberedEntityTypesOf() gives, from 1.
 */
struct NumberedEntityType {
  std::string name;
  EntityType side{};
  CollisionBox hitbox{};
};

}  // namespace rtype::game
