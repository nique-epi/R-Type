#pragma once

#include <functional>
#include <optional>
#include <span>
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "ObstacleContact.hpp"

namespace rtype::client {

/**
 * @brief What the client needs to create an entity of one type: its side,
 * which picks its layer, its sounds and its effects, the appearance of its
 * sheet, which the client owns for as long as it runs, its box, and the boxes
 * its sheet lists, none for a body of one box, which the client tests its own
 * missiles against and owns as long as the appearance.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EntityTypeLook {
  game::EntityType side{};
  std::reference_wrapper<const EntityAppearance> appearance;
  game::CollisionBox hitbox{};
  std::span<const game::CollisionBox> hitboxParts;
  /** @brief What it does to a ship, for an obstacle, which the replicator
   * gives it as its game::Obstacle; nothing for any other body. */
  std::optional<game::ObstacleContact> obstacle = std::nullopt;
};

}  // namespace rtype::client
