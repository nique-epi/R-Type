#pragma once

#include "EntitySide.hpp"
#include "EntityType.hpp"

namespace rtype::server {

/**
 * @brief The side the protocol writes for a body of side @p type in the table
 * of the entity types.
 */
[[nodiscard]] network::EntitySide toEntitySide(game::EntityType type);

}  // namespace rtype::server
