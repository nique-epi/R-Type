#pragma once

#include <functional>
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"

namespace rtype::client {

/**
 * @brief What the client needs to create an entity of one type: its side,
 * which picks its layer, its sounds and its effects, the appearance of its
 * sheet, which the client owns for as long as it runs, and its box, which the
 * client tests its own missiles against.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EntityTypeLook {
  game::EntityType side{};
  std::reference_wrapper<const EntityAppearance> appearance;
  game::CollisionBox hitbox{};
};

}  // namespace rtype::client
