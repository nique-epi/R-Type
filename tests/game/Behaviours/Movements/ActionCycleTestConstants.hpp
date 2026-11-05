#pragma once

#include <chrono>
#include "ActionKey.hpp"
#include "ActionTimings.hpp"
#include "EnemyPhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief An attack with every phase, timed like the orb of the Bydomancer:
 * a 300 ms signal, a 120 ms strike, a 300 ms recover. */
constexpr ActionTimings FULL_ATTACK{.attack = 0,
                                    .signal = std::chrono::milliseconds(300),
                                    .strike = std::chrono::milliseconds(120),
                                    .recover = std::chrono::milliseconds(300)};

/** @brief An attack with a strike only, like the nova of the Bydomancer. */
constexpr ActionTimings STRIKE_ONLY_ATTACK{
    .attack = 1,
    .signal = engine::Duration::zero(),
    .strike = std::chrono::milliseconds(240),
    .recover = engine::Duration::zero()};

/** @brief The phases of FULL_ATTACK and STRIKE_ONLY_ATTACK. */
constexpr ActionKey FULL_SIGNAL{.attack = 0, .phase = EnemyPhase::Signal};
constexpr ActionKey FULL_STRIKE{.attack = 0, .phase = EnemyPhase::Strike};
constexpr ActionKey FULL_RECOVER{.attack = 0, .phase = EnemyPhase::Recover};
constexpr ActionKey LONE_STRIKE{.attack = 1, .phase = EnemyPhase::Strike};

/** @brief One tick less than a phase, the last age at which it goes on. */
constexpr engine::Duration ONE_TICK = engine::SIMULATION_TICK_DURATION;

/** @brief A rest shorter than what the repeat window asks of FULL_ATTACK
 * (2000 - 720 = 1280 ms), and one longer than it. */
constexpr engine::Duration SHORT_REST = std::chrono::milliseconds(500);
constexpr engine::Duration LONG_REST = std::chrono::milliseconds(1500);

/** @brief The idle time the repeat window asks after FULL_ATTACK: two seconds
 * from its signal, minus its 720 ms. */
constexpr engine::Duration WINDOW_REST_AFTER_FULL_ATTACK =
    std::chrono::milliseconds(1280);

}  // namespace rtype::game::testing
