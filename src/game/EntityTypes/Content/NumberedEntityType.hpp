#pragma once

#include <memory>
#include <string>
#include <vector>
#include "CollisionBox.hpp"
#include "EntityType.hpp"
#include "PixelMask.hpp"

namespace rtype::game {

/**
 * @brief What a client knows of an entity type it draws: the name of its
 * sheet, its side, its box, the boxes its sheet lists, none for a body of one
 * box, and its collision mask, none for a sheet without one, which the client
 * tests its own missiles against as the server does. Its number is its place
 * in the list numberedEntityTypesOf() gives, from 1.
 */
struct NumberedEntityType {
  std::string name;
  EntityType side{};
  CollisionBox hitbox{};
  std::vector<CollisionBox> hitboxParts;
  std::shared_ptr<const engine::PixelMask> mask;
};

}  // namespace rtype::game
