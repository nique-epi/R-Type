#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The settings of the shipped Drillback: a drift of 120 units a
 * second, a charge left of 1824, a rest of 600 ms, a shaking of 600 ms, a
 * charge of 700 ms and a stun of 900 ms; and its top speed, the speed of its
 * charge, 280 sheet pixels a second. */
constexpr std::array<std::int64_t, 6> DRILLBACK_SETTINGS_OF_THE_SHEET{
    120, 1824, 600, 600, 700, 900};
constexpr float DRILLBACK_TOP_SPEED = 1120.0F;

/** @brief The share of its top speed its drift is: 120 / 1120. */
constexpr float DRIFT_SHARE = -120.0F / 1120.0F;

/** @brief A Drillback still past the right edge of where it may charge, and
 * one inside it. */
constexpr Position NOT_IN_YET{.x = 1900.0F, .y = 500.0F};
constexpr Position IN_PLAY{.x = 1800.0F, .y = 500.0F};

/** @brief How long it has been idle, long enough to charge. */
constexpr engine::Duration DRILLBACK_RESTED = std::chrono::seconds(2);

/** @brief The phases of its charge, and how long the charge lasts. */
constexpr ActionKey SHAKING{.attack = 0, .phase = EnemyPhase::Signal};
constexpr ActionKey CHARGE{.attack = 0, .phase = EnemyPhase::Strike};
constexpr ActionKey STUN{.attack = 0, .phase = EnemyPhase::Recover};
constexpr engine::Duration CHARGE_TIME = std::chrono::milliseconds(700);

}  // namespace rtype::game::testing
