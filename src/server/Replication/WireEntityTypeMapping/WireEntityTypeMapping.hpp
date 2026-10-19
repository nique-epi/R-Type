#pragma once

#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::server {

/**
 * @brief The type the protocol names an entity of type @p type by.
 */
[[nodiscard]] network::WireEntityType toWireEntityType(game::EntityType type);

}  // namespace rtype::server
