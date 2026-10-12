#pragma once

#include "EntityType.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/** @brief An entity of the sprite showcase, and how far across the playfield it
 * stands, from 0 (left edge) to 1 (right edge). */
struct ShowcaseEntity {
  game::EntityType type;
  float shareOfPlayfieldWidth;
};

/**
 * @brief Creates one entity of each type in a row across the middle of the
 * playfield, each with the sprite the entity table gives its type.
 *
 * Provisional: it shows the sprites on screen until the client creates the
 * entities the server announces, then it is removed.
 */
void spawnSpriteShowcase(engine::EntityRegistry& entities,
                         engine::ComponentRegistry& components);

}  // namespace rtype::client
