#pragma once

#include <cstddef>
#include <optional>
#include "CollisionBox.hpp"
#include "HurtBox.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief The boxes of the shipped ship and missile of an enemy. */
constexpr CollisionBox SHIP_BOX{.width = 128.0F, .height = 64.0F};
constexpr CollisionBox ENEMY_MISSILE_BOX{.width = 32.0F, .height = 32.0F};

/** @brief Where the ship of the tests is, and a place 200 units higher, past
 * the 64 units of the ship and the 32 of a missile. */
constexpr Position WHERE_THE_SHIP_IS{.x = 400.0F, .y = 300.0F};
constexpr Position TWO_HUNDRED_UNITS_HIGHER{.x = 400.0F, .y = 100.0F};

/** @brief A box of a HurtZone, 32 by 32 on the enemy, hurting all the time. */
constexpr HurtBox ALWAYS_HURTING_BOX{.offsetX = 0.0F,
                                     .offsetY = 0.0F,
                                     .width = 32.0F,
                                     .height = 32.0F,
                                     .activeIn = std::nullopt,
                                     .window = {}};

/** @brief A view five ticks behind the present, and the ticks the threat of a
 * test spends at its first place, then at its second, so that five ticks
 * behind the present lands on its first place. */
constexpr std::size_t SHIP_FIVE_TICKS_BEHIND = 5;
constexpr std::size_t TICKS_AT_THE_FIRST_PLACE = 1;
constexpr std::size_t TICKS_AT_THE_SECOND_PLACE = 5;

}  // namespace rtype::game::testing
