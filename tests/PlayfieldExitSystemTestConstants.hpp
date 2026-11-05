#pragma once

#include <cstdint>
#include "CollisionBox.hpp"

namespace rtype::game::testing {

/** @brief The row the exit tests place their entities on. */
constexpr float EXIT_ROW_Y = 300.0F;
/** @brief Where an enemy is just past its exit margin of 64 units beyond the
 * left edge, exactly at it, and far past the edge. */
constexpr float JUST_PAST_THE_MARGIN_X = -65.0F;
constexpr float EXACTLY_AT_THE_MARGIN_X = -64.0F;
constexpr float FAR_PAST_THE_EDGE_X = -1000.0F;
/**
 * @brief Where a missile of a player stands against the right edge of the
 * playfield, at 1920: its center is half of its 64 units past the edge at 1952,
 * when the whole missile just left; one unit further, it is out.
 */
constexpr float MISSILE_EXACTLY_AT_THE_MARGIN_X = 1952.0F;
constexpr float MISSILE_JUST_PAST_THE_MARGIN_X = 1953.0F;
/**
 * @brief Where a missile of an enemy stands against the left edge of the
 * playfield, at 0: its center is half of its 32 units past the edge at -16,
 * when the whole missile just left; one unit further, it is out.
 */
constexpr float ENEMY_MISSILE_EXACTLY_AT_THE_MARGIN_X = -16.0F;
constexpr float ENEMY_MISSILE_JUST_PAST_THE_MARGIN_X = -17.0F;
/** @brief Where a new enemy appears, 64 units past the right edge. */
constexpr float WHERE_ENEMIES_APPEAR_X = 1984.0F;
/** @brief The box of a missile of a player and of an enemy, as their sprites
 * have it: 16 by 6 and 8 by 8 pixels, each covering 4 logical units. */
constexpr CollisionBox PLAYER_MISSILE_EXIT_BOX{.width = 64.0F, .height = 24.0F};
constexpr CollisionBox ENEMY_MISSILE_EXIT_BOX{.width = 32.0F, .height = 32.0F};
/** @brief A box of a missile wider than the one of a missile of an enemy: 40
 * units instead of 32, so half of it is 20 units instead of 16. */
constexpr CollisionBox WIDER_MISSILE_BOX{.width = 40.0F, .height = 32.0F};
/** @brief The network id of every entity the exit tests place. */
constexpr std::uint32_t EXITING_NETWORK_ID = 7;

}  // namespace rtype::game::testing
