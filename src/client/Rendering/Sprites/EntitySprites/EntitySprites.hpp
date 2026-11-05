#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include "AnimatedSprite.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @returns The layer an entity of side @p side is drawn in: a ship and the
 * missile of a player in front of the effects, the missile of an enemy in
 * front of everything of the playfield, an enemy behind the effects. The side
 * decides it, never the sheet, so the order that keeps danger in sight holds
 * for every type a sheet adds.
 */
[[nodiscard]] game::Layer layerOf(game::EntityType side);

/**
 * @brief What code that creates an entity gives it: the idle animation of
 * @p appearance in a loop, started at an offset taken from @p networkId, so
 * entities created in the same tick do not show the same frame, and a sprite
 * of the sheet of @p appearance, in the layer of @p side, already showing the
 * frame that animation starts on, so the first draw of the entity is right.
 *
 * The offset is the fractional part of networkId times LOOP_START_SPREAD, as a
 * share of one cycle of the animation: 0 for id 0, always less than one cycle.
 * The animation views the frames of @p appearance, which must outlive it.
 */
[[nodiscard]] AnimatedSprite animatedSpriteOf(
    const EntityAppearance& appearance, game::EntityType side,
    std::uint32_t networkId);

/** @returns The rectangle of a frame of a sheet, as SFML takes it. */
[[nodiscard]] sf::IntRect rectangleOf(game::SpriteFrame frame);

/**
 * @returns A frame of the ship of the first player color, moved down the
 * entity sheet to the row of another color, 0 to PLAYER_COLOR_COUNT - 1.
 */
[[nodiscard]] game::SpriteFrame inPlayerColorRow(game::SpriteFrame frame,
                                                 int color);

}  // namespace rtype::client
