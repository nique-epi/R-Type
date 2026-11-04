#pragma once

#include <optional>
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::server {

/**
 * @brief The type the protocol names an entity of type @p type by.
 *
 * @p kind is read for an EntityType::Enemy only: Drifter and Weaver are both
 * a Bydo, every other kind has a type of its own. An enemy without a kind (an
 * entity not built by World::spawnBydo) is a Bydo. Pass std::nullopt for any
 * other type.
 */
[[nodiscard]] network::WireEntityType toWireEntityType(
    game::EntityType type, std::optional<game::EnemyKind> kind);

}  // namespace rtype::server
