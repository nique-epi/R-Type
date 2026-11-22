#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <span>
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "HurtZone.hpp"
#include "ObstacleContact.hpp"
#include "PixelMask.hpp"

namespace rtype::client {

/**
 * @brief What the client needs to create an entity of one type: its side,
 * which picks its layer, its sounds and its effects, the appearance of its
 * sheet, which the client owns for as long as it runs, its box, the boxes its
 * sheet lists, none for a body of one box, and the collision mask of its
 * sheet, none for a sheet without one, which the client tests its own missiles
 * against and owns as long as the appearance, and its hurt zone, whose
 * beams the client paints for a sheet that asks for it.
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
  std::shared_ptr<const engine::PixelMask> mask;
  /** @brief What it does to a ship, for an obstacle, which the replicator
   * gives it as its game::Obstacle; nothing for any other body. */
  std::optional<game::ObstacleContact> obstacle = std::nullopt;
  /** @brief Its hurt zone, which the replicator copies into a body whose
   * appearance paints its hurt beams (PaintedBeams), owned by the table of
   * types as long as the appearance; nullptr for a sheet without one. */
  const game::HurtZone* hurtZone = nullptr;
};

}  // namespace rtype::client
