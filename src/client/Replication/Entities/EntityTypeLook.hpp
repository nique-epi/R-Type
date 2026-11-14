#pragma once

#include <functional>
#include <memory>
#include <span>
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "PixelMask.hpp"

namespace rtype::client {

/**
 * @brief What the client needs to create an entity of one type: its side,
 * which picks its layer, its sounds and its effects, the appearance of its
 * sheet, which the client owns for as long as it runs, its box, the boxes its
 * sheet lists, none for a body of one box, and the collision mask of its
 * sheet, none for a sheet without one, which the client tests its own missiles
 * against and owns as long as the appearance.
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
};

}  // namespace rtype::client
