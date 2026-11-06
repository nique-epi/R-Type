#pragma once

#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityFeedback.hpp"

namespace rtype::client {

/**
 * @brief Plays the appearance of @p entity: the arrival of a ship, the muzzle
 * flash and the shot of a missile, nothing for a Bydo. Does nothing for an
 * entity without an EntityType or a Position.
 */
void announce(const EntityFeedback& feedback,
              engine::ComponentRegistry& components, engine::Entity entity);

/**
 * @brief Plays the explosion of @p entity, centered where it stands, and its
 * sound: T4 for a ship. Does nothing for an entity without an EntityType or a
 * Position, and for one still awaiting its arrival, which was never shown.
 */
void explode(const EntityFeedback& feedback,
             engine::ComponentRegistry& components, engine::Entity entity);

}  // namespace rtype::client
