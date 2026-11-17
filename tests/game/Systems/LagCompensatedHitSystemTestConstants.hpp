#pragma once

#include <array>
#include <cstddef>
#include "CollisionBox.hpp"
#include "LagCompensationConstants.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief Where the enemy of the tests stands, then where it went: 200 units
 * higher, past the 96 by 96 box of an enemy and the 24 units of a missile. */
constexpr Position WHERE_THE_PLAYER_SAW_THE_ENEMY{.x = 500.0F, .y = 300.0F};
constexpr Position WHERE_THE_ENEMY_WENT{.x = 500.0F, .y = 100.0F};

/** @brief The boxes of the shipped enemy and player missile. */
constexpr CollisionBox ENEMY_BOX{.width = 96.0F, .height = 96.0F};
constexpr CollisionBox MISSILE_BOX{.width = 64.0F, .height = 24.0F};

/** @brief A view five ticks behind the present, and the furthest the system
 * keeps with the tick just past it. */
constexpr std::size_t FIVE_TICKS_BEHIND = 5;
constexpr std::size_t KEPT_TICKS_BEHIND = MAXIMUM_REWIND_TICKS;
constexpr std::size_t PAST_THE_KEPT_TICKS_BEHIND = MAXIMUM_REWIND_TICKS + 1;

/** @brief A view a client may claim far beyond what the system keeps, eight
 * ticks further, and the ticks the enemy spends gone before it stands at its
 * place, then after: the place is 7 ticks behind the present, within what a
 * client may claim but not where the system looks for it. */
constexpr std::size_t FAR_CLAIM_TICKS_BEHIND = MAXIMUM_REWIND_TICKS + 8;
constexpr std::size_t TICKS_GONE_BEFORE = MAXIMUM_REWIND_TICKS;
constexpr std::size_t TICKS_GONE_AFTER = 6;

/** @brief The two boxes of a tall enemy, each the box of an enemy, 200 units
 * above and 200 below its Position: an enemy at
 * WHERE_THE_PLAYER_SAW_THE_ENEMY has its lower box on
 * WHERE_ITS_LOWER_BOX_IS, and nothing from y 148 to 452, where a missile of
 * 24 units on its Position flies through. */
constexpr float TALL_ENEMY_PART_DISTANCE = 200.0F;
constexpr std::array TALL_ENEMY_PARTS{
    CollisionBox{
        .width = 96.0F, .height = 96.0F, .offsetY = -TALL_ENEMY_PART_DISTANCE},
    CollisionBox{
        .width = 96.0F, .height = 96.0F, .offsetY = TALL_ENEMY_PART_DISTANCE}};
constexpr Position WHERE_ITS_LOWER_BOX_IS{.x = 500.0F, .y = 500.0F};

/**
 * @brief The mask of a hollow enemy, as large as ENEMY_BOX (96 / 4 = 24
 * pixels a side), its HOLLOW_ENEMY_DRAWN_ROWS top rows alone drawn. At
 * WHERE_THE_PLAYER_SAW_THE_ENEMY it lies on cells 113 to 136 and 63 to 86
 * (500 / 4 - 12 and 300 / 4 - 12), its drawn rows on 63 to 66, from y 252 to
 * 268: a missile on its Position (y 288 to 312) crosses its clear rows only,
 * and one centered at WHERE_A_MISSILE_GRAZES_ITS_TOP (y 248 to 272) crosses
 * its drawn rows.
 */
constexpr int HOLLOW_ENEMY_SIDE = 24;
constexpr int HOLLOW_ENEMY_DRAWN_ROWS = 4;
constexpr Position WHERE_A_MISSILE_GRAZES_ITS_TOP{.x = 500.0F, .y = 260.0F};

}  // namespace rtype::game::testing
