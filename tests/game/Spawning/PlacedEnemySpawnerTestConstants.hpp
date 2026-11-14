#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief The network id the recorded orders give every body. */
constexpr std::uint32_t SPAWNED_ENEMY_ID = 12;

/** @brief An enemy placed on the first screen of the level, at x 800: it
 * comes in before the level scrolls, at its place. */
constexpr Position ON_THE_FIRST_SCREEN{.x = 800.0F, .y = 540.0F};
constexpr float NO_SCROLL = 0.0F;

/** @brief An enemy placed at x 2224: its left edge reaches the right edge of
 * the playfield (1920) once the level scrolled 256 units, 2224 - 1920 - 48,
 * where it appears at x 1968. One tick of scroll earlier, at 254, it is not
 * in yet. */
constexpr Position PAST_THE_FIRST_SCREEN{.x = 2224.0F, .y = 540.0F};
constexpr float SCROLL_ONE_TICK_BEFORE_IT_COMES_IN = 254.0F;
constexpr float SCROLL_WHEN_IT_COMES_IN = 256.0F;
constexpr float WHERE_IT_APPEARS_X = 1968.0F;
constexpr float SCROLL_PAST_IT = 300.0F;

/** @brief A wide enemy, 400 units, at x 2300: its left edge comes in at 180
 * units of scroll, before the one at 2224 does. */
constexpr Position WIDE_ENEMY_CENTER{.x = 2300.0F, .y = 200.0F};
constexpr float WIDE_WIDTH = 400.0F;

/** @brief Two enemies coming in at 100 and 300 units of scroll, a checkpoint
 * between them at 200, and the scroll past both at 400. */
constexpr Position COMES_IN_AT_100{.x = 2068.0F, .y = 300.0F};
constexpr Position COMES_IN_AT_300{.x = 2268.0F, .y = 700.0F};
constexpr float CHECKPOINT_SCROLL = 200.0F;
constexpr float SCROLL_PAST_BOTH = 400.0F;

/** @brief An enemy coming in exactly at the checkpoint, 200, and one coming
 * in one tick after it, at 202. */
constexpr Position COMES_IN_AT_THE_CHECKPOINT{.x = 2168.0F, .y = 500.0F};
constexpr Position COMES_IN_ONE_TICK_AFTER_THE_CHECKPOINT{.x = 2170.0F,
                                                          .y = 500.0F};

}  // namespace rtype::game::testing
