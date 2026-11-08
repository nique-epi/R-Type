#pragma once

#include <cstddef>
#include <optional>
#include "ActionKey.hpp"
#include "CollisionBox.hpp"
#include "EnemyPhase.hpp"
#include "HurtBeam.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
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

/** @brief The strike of a beam attack of the tests, and a beam of 400 by 20
 * units heading left from the enemy in it: an enemy at BEAM_ENEMY_AT lays it
 * from x 400 to 800 on the line of the ship, and 200 units higher it passes
 * above the ship. */
constexpr ActionKey BEAM_ATTACK_STRIKE{.attack = 2,
                                       .phase = EnemyPhase::Strike};
constexpr HurtBeam LEFTWARD_BEAM{.offsetX = 0.0F,
                                 .offsetY = 0.0F,
                                 .length = 400.0F,
                                 .thickness = 20.0F,
                                 .fromDegrees = 0.0F,
                                 .toDegrees = 0.0F,
                                 .activeIn = BEAM_ATTACK_STRIKE,
                                 .window = HurtWindow{}};
constexpr Position BEAM_ENEMY_AT{.x = 800.0F, .y = 300.0F};
constexpr Position BEAM_ENEMY_HIGHER{.x = 800.0F, .y = 100.0F};

/** @brief A view five ticks behind the present, and the ticks the threat of a
 * test spends at its first place, then at its second, so that five ticks
 * behind the present lands on its first place. */
constexpr std::size_t SHIP_FIVE_TICKS_BEHIND = 5;
constexpr std::size_t TICKS_AT_THE_FIRST_PLACE = 1;
constexpr std::size_t TICKS_AT_THE_SECOND_PLACE = 5;

}  // namespace rtype::game::testing
