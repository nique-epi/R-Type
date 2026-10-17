#pragma once

#include <cstdint>
#include <optional>
#include "EntityType.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"

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

/**
 * @brief The animation of each entity type, from the same table: the frames of
 * its row in a loop, started at an offset taken from the network id of the
 * entity, so entities created in the same tick do not show the same frame.
 *
 * The offset is the fractional part of networkId times LOOP_START_SPREAD, as a
 * share of one cycle of the animation: 0 for id 0, always less than one cycle.
 *
 * @returns The animation, its elapsed time set to the offset, or nothing for a
 * value outside EntityType.
 */
[[nodiscard]] std::optional<SpriteAnimation> animationOf(
    game::EntityType type, std::uint32_t networkId);

}  // namespace rtype::client
