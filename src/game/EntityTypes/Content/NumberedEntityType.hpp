#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "CollisionBox.hpp"
#include "EntityType.hpp"
#include "ObstacleContact.hpp"
#include "PixelMask.hpp"

namespace rtype::game {

/**
 * @brief What a client knows of an entity type it draws: the name of its
 * sheet, its side, its box, the boxes its sheet lists, none for a body of one
 * box, and its collision mask, none for a sheet without one, which the client
 * tests its own missiles against as the server does, and what an obstacle
 * does to a ship. Its number is its place in the list
 * numberedEntityTypesOf() gives, from 1.
 */
struct NumberedEntityType {
  std::string name;
  EntityType side{};
  CollisionBox hitbox{};
  std::vector<CollisionBox> hitboxParts;
  std::shared_ptr<const engine::PixelMask> mask;
  /** @brief What it does to a ship that touches it, for a body of side
   * obstacle: the client predicts its own ship against a blocking one; nothing
   * for any other body. */
  std::optional<ObstacleContact> obstacle{std::nullopt};
};

}  // namespace rtype::game
