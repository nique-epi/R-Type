#pragma once

#include <cstddef>
#include "CollisionBox.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief Where the enemy of the tests stands, then where it went: 200 units
 * higher, past the 96 by 96 box of an enemy and the 24 units of a missile. */
constexpr Position WHERE_THE_PLAYER_SAW_THE_ENEMY{.x = 500.0F, .y = 300.0F};
constexpr Position WHERE_THE_ENEMY_WENT{.x = 500.0F, .y = 100.0F};

/** @brief The boxes of the shipped enemy and player missile. */
constexpr CollisionBox ENEMY_BOX{.width = 96.0F, .height = 96.0F};
constexpr CollisionBox MISSILE_BOX{.width = 64.0F, .height = 24.0F};

/** @brief A view five ticks behind the present, and the cap of the rewind
 * with the tick just past it. */
constexpr std::size_t FIVE_TICKS_BEHIND = 5;
constexpr std::size_t TWELVE_TICKS_BEHIND = 12;
constexpr std::size_t THIRTEEN_TICKS_BEHIND = 13;

/** @brief A view a client may claim far beyond the cap, 20 ticks behind, and
 * the ticks the enemy spends gone before it stands at its place, then after:
 * the place is 7 ticks behind the present, within what a client may claim but
 * past what the server looks back for it. */
constexpr std::size_t TWENTY_TICKS_BEHIND = 20;
constexpr std::size_t TICKS_GONE_BEFORE = 5;
constexpr std::size_t TICKS_GONE_AFTER = 6;

}  // namespace rtype::game::testing
