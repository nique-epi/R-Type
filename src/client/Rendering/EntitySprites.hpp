#pragma once

#include <optional>
#include "EntityType.hpp"
#include "Sprite.hpp"

namespace rtype::client {

/**
 * @brief The single table that gives each entity type its look on screen.
 *
 * Every type is drawn from the entity sheet, in the entities layer. All four
 * players get the first ship color.
 *
 * @returns The sprite of the type, or nothing for a value outside EntityType.
 */
[[nodiscard]] std::optional<game::Sprite> spriteOf(game::EntityType type);

}  // namespace rtype::client
