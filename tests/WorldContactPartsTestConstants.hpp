#pragma once

#include <cstdint>
#include <string_view>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"

namespace rtype::game::testing {

/** @brief The hurt zone the contact parts tests give a sheet: a box 48 units
 * ahead, 64 by 32, that hurts all the time, and a box 16 units below, 40 by
 * 24, that hurts in the strike of attack 3. */
constexpr float ALWAYS_BOX_OFFSET_X = 48.0F;
constexpr float ALWAYS_BOX_WIDTH = 64.0F;
constexpr float ALWAYS_BOX_HEIGHT = 32.0F;
constexpr float STRIKE_BOX_OFFSET_Y = 16.0F;
constexpr float STRIKE_BOX_WIDTH = 40.0F;
constexpr float STRIKE_BOX_HEIGHT = 24.0F;
constexpr std::uint8_t STRIKE_BOX_ATTACK = 3;

/** @brief The action a fragile sheet of the tests is fragile in. */
constexpr ActionKey STUNNED_IN_ATTACK_ZERO{.attack = 0,
                                           .phase = EnemyPhase::Recover};

/** @brief A name SHOOTABLE_MISSILE_SHEETS does not hold, given to a copy of
 * the sheet of a missile of an enemy. */
constexpr std::string_view UNLISTED_MISSILE_SHEET = "unlisted_missile";

}  // namespace rtype::game::testing
