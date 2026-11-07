#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief The settings of the shipped Bydo Tyrant, in the order of its
 * behaviour: it rests at column 1616 for 1000 ms between attacks; slides in
 * over 1800 ms, roars 800 ms, settles 0; signals, strikes and recovers its
 * burst 600/200/300, its beam 800/600/300, its brood 800/200/300, its
 * tentacles 300/400/300; lashes them at (-136, 168) from its center within
 * 248 units; holds its rage 700 ms; keeps 70 % of a calm signal enraged;
 * sweeps 560/1200/300, lays its spores 560/200/300, its wall 560/200/300,
 * and charges 700/1600/1000. Its top speed is 2500 units a second.
 */
constexpr std::array<std::int64_t, 34> TYRANT_SETTINGS_OF_THE_SHEET{
    1616, 1000, 1800, 800, 0,   600,  200, 300, 800,  600, 300, 800,
    200,  300,  300,  400, 300, -136, 168, 248, 700,  70,  560, 1200,
    300,  560,  200,  300, 560, 200,  300, 700, 1600, 1000};
constexpr float TYRANT_TOP_SPEED = 2500.0F;
/** @brief How far it moves in one tick at a full share: 2500 / 60 units. */
constexpr float TYRANT_REACH_IN_A_TICK = 2500.0F / 60.0F;
constexpr float TYRANT_COLUMN_TOLERANCE = 1e-2F;

/** @brief Where it appears, half its frame past the right edge on the middle
 * row of the area of play, and where it rests. */
constexpr Position WHERE_THE_TYRANT_APPEARS{.x = 2176.0F, .y = 492.0F};
constexpr Position TYRANT_AT_REST{.x = 1616.0F, .y = 492.0F};

/** @brief A ship far from it, and one on its melee point (1480, 660). */
constexpr Position SHIP_FAR_FROM_THE_TYRANT{.x = 200.0F, .y = 492.0F};
constexpr Position SHIP_UNDER_ITS_JAW{.x = 1480.0F, .y = 660.0F};

/** @brief How long it has been idle: rested, or short of its rest. */
constexpr engine::Duration TYRANT_RESTED = std::chrono::milliseconds(1000);
constexpr engine::Duration TYRANT_SHORT_OF_ITS_REST =
    std::chrono::milliseconds(500);
/** @brief An age long past its slide. */
constexpr engine::Duration TYRANT_LONG_IN_PLAY = std::chrono::seconds(30);

/** @brief Its arrival: halfway through its slide it heads for column 1896,
 * halfway from 2176 to 1616; the slide lasts 1800 ms. */
constexpr ActionKey ARRIVAL_SIGNAL{.attack = 0, .phase = EnemyPhase::Signal};
constexpr ActionKey ARRIVAL_STRIKE{.attack = 0, .phase = EnemyPhase::Strike};
constexpr engine::Duration HALFWAY_THROUGH_THE_SLIDE =
    std::chrono::milliseconds(900);
constexpr float HALFWAY_THROUGH_THE_SLIDE_X = 1896.0F;
constexpr engine::Duration WHOLE_SLIDE = std::chrono::milliseconds(1800);

/** @brief Its calm attacks: the burst opens, the tentacles lash. */
constexpr ActionKey BURST_SIGNAL{.attack = 1, .phase = EnemyPhase::Signal};
constexpr ActionKey BURST_STRIKE{.attack = 1, .phase = EnemyPhase::Strike};
constexpr ActionKey TENTACLE_SIGNAL{.attack = 4, .phase = EnemyPhase::Signal};
constexpr std::uint32_t ITS_ARRIVAL_ENDED = 1;

/** @brief Its enraged attacks: the burst signals 420 ms, 70 % of 600; the
 * tentacles; the sweep, third of its enraged rotation after two actions. */
constexpr ActionKey ENRAGED_BURST_SIGNAL{.attack = 11,
                                         .phase = EnemyPhase::Signal};
constexpr ActionKey ENRAGED_BURST_STRIKE{.attack = 11,
                                         .phase = EnemyPhase::Strike};
constexpr engine::Duration ENRAGED_BURST_SIGNAL_LENGTH =
    std::chrono::milliseconds(420);
constexpr engine::Duration JUST_SHORT_OF_THE_ENRAGED_BURST_SIGNAL =
    std::chrono::milliseconds(419);
constexpr ActionKey ENRAGED_TENTACLE_SIGNAL{.attack = 14,
                                            .phase = EnemyPhase::Signal};
constexpr ActionKey SWEEP_SIGNAL{.attack = 15, .phase = EnemyPhase::Signal};
constexpr std::uint32_t TWO_ENDED_ACTIONS = 2;

/** @brief Its rage, held 700 ms. */
constexpr ActionKey RAGE{.attack = 9, .phase = EnemyPhase::Enraged};
constexpr engine::Duration INTO_ITS_RAGE = std::chrono::milliseconds(300);
constexpr engine::Duration WHOLE_RAGE = std::chrono::milliseconds(700);

/**
 * @brief Its charge, a strike of 1600 ms: 400 ms in, a quarter of it, it
 * heads for column 648, halfway from 1616 to -320; 1000 ms in, it comes back
 * from column 2240, a quarter of the way to 1616: column 2084, which it leaps
 * to from past the left edge.
 */
constexpr ActionKey CHARGE_STRIKE{.attack = 18, .phase = EnemyPhase::Strike};
constexpr engine::Duration A_QUARTER_INTO_THE_CHARGE =
    std::chrono::milliseconds(400);
constexpr float A_QUARTER_INTO_THE_CHARGE_X = 648.0F;
constexpr engine::Duration PAST_THE_TURN_OF_THE_CHARGE =
    std::chrono::milliseconds(1000);
constexpr Position TYRANT_PAST_THE_LEFT_EDGE{.x = -320.0F, .y = 492.0F};
constexpr Position WHERE_THE_CHARGE_COMES_BACK{.x = 2084.0F, .y = 492.0F};
/** @brief Past the turn of its charge but late: its center at -100, so
 * the right 156 units of its 512-unit body still show. */
constexpr Position TYRANT_STILL_SHOWING_ON_THE_LEFT{.x = -100.0F, .y = 492.0F};

/** @brief An attack it does not know. */
constexpr ActionKey UNKNOWN_TYRANT_ATTACK{.attack = 7,
                                          .phase = EnemyPhase::Signal};

}  // namespace rtype::game::testing
