#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <optional>
#include "AnimatedSprite.hpp"
#include "EntityType.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"

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

/**
 * @brief What code that creates an entity gives it: the animation of
 * animationOf(), and the sprite of spriteOf() showing the frame that animation
 * starts on, so the first draw of the entity is already right.
 *
 * @returns Both, or nothing for a value outside EntityType.
 */
[[nodiscard]] std::optional<AnimatedSprite> animatedSpriteOf(
    game::EntityType type, std::uint32_t networkId);

/** @returns The rectangle of a frame of a sheet, as SFML takes it. */
[[nodiscard]] sf::IntRect rectangleOf(game::SpriteFrame frame);

/**
 * @returns A frame of the ship of the first player color, moved down the
 * entity sheet to the row of another color, 0 to PLAYER_COLOR_COUNT - 1.
 */
[[nodiscard]] game::SpriteFrame inPlayerColorRow(game::SpriteFrame frame,
                                                 int color);

}  // namespace rtype::client
