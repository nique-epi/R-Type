#pragma once

#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The network id of the enemy whose beams are painted, and one the
 * client does not have. */
constexpr std::uint32_t BEAMING_ENEMY_ID = 31;
constexpr std::uint32_t UNKNOWN_ENEMY_ID = 32;

/** @brief Its beam: the signal of attack 2, then its strike. */
constexpr std::uint8_t BEAM_ATTACK_NUMBER = 2;
constexpr game::ActionKey BEAM_SIGNAL_KEY{.attack = BEAM_ATTACK_NUMBER,
                                          .phase = game::EnemyPhase::Signal};
constexpr game::ActionKey BEAM_STRIKE_KEY{.attack = BEAM_ATTACK_NUMBER,
                                          .phase = game::EnemyPhase::Strike};

/**
 * @brief Server ticks: the signal announced at 90, the strike at 100; the
 * client draws tick 95 (5 ticks into the signal), 99.5 (before the strike),
 * 101 (1 tick into it) and 106 (6 ticks into it).
 */
constexpr double SIGNAL_ANNOUNCED_AT = 90.0;
constexpr double STRIKE_ANNOUNCED_AT = 100.0;
constexpr double DRAWN_INTO_THE_SIGNAL = 95.0;
constexpr double DRAWN_BEFORE_THE_STRIKE = 99.5;
constexpr double DRAWN_JUST_INTO_THE_STRIKE = 101.0;
constexpr double DRAWN_INTO_THE_STRIKE = 106.0;
constexpr double FIRST_DRAWN_TICK = 0.0;
constexpr engine::Duration ONE_TICK = engine::SIMULATION_TICK_DURATION;
constexpr engine::Duration FIVE_TICKS = engine::SIMULATION_TICK_DURATION * 5;
constexpr engine::Duration SIX_TICKS = engine::SIMULATION_TICK_DURATION * 6;

}  // namespace rtype::client::testing
