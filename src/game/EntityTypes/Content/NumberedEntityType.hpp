#pragma once

#include <string>
#include <vector>
#include "CollisionBox.hpp"
#include "EntityType.hpp"

namespace rtype::game {

/**
 * @brief What a client knows of an entity type it draws: the name of its
 * sheet, its side, its box and the boxes its sheet lists, none for a body of
 * one box, which the client tests its own missiles against. Its number is its
 * place in the list numberedEntityTypesOf() gives, from 1.
 */
struct NumberedEntityType {
  std::string name;
  EntityType side{};
  CollisionBox hitbox{};
  std::vector<CollisionBox> hitboxParts;
};

}  // namespace rtype::game
