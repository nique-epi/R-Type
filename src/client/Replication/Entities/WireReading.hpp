#pragma once

#include <optional>
#include "ActionKey.hpp"
#include "EnemyAction.hpp"
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/** @brief What the client makes of a type the protocol names: its game type,
 * and for an enemy its kind. */
struct AnnouncedEntity {
  game::EntityType type{game::EntityType::Player};
  std::optional<game::EnemyKind> kind;
};

/**
 * @returns What an entity of the protocol type @p wireType is, or nothing for
 * a value outside WireEntityType. The inverse of toWireEntityType on the
 * server: a Bydo is read as a Drifter (it is drawn alike a Weaver), and the
 * projectiles of the new enemies are read as an EnemyMissile and drawn as one
 * until their own pull request.
 */
[[nodiscard]] std::optional<AnnouncedEntity> announcedEntityOf(
    network::WireEntityType wireType);

/** @returns The attack and phase an EnemyAction names. */
[[nodiscard]] game::ActionKey actionKeyOf(const network::EnemyAction& message);

}  // namespace rtype::client
