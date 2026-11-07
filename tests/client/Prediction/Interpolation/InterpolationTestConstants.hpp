#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "Position.hpp"
#include "PositionSample.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

constexpr engine::Duration ONE_TICK = engine::SIMULATION_TICK_DURATION;
constexpr engine::Duration TEN_SECONDS = std::chrono::seconds(10);

/** @brief Ticks of the states of the clock tests: every 6 ticks, which is 10
 * states per second at 60 ticks per second. */
constexpr std::uint32_t FIRST_STATE_TICK = 60;
constexpr std::uint32_t SECOND_STATE_TICK = 66;
constexpr std::uint32_t THIRD_STATE_TICK = 72;
constexpr std::uint32_t FOURTH_STATE_TICK = 78;
constexpr std::uint32_t FIFTH_STATE_TICK = 84;
constexpr std::uint32_t TICKS_BETWEEN_STATES = 6;
/** @brief 20 states per second: 3 ticks apart. */
constexpr std::uint32_t TICKS_BETWEEN_STATES_AT_TWENTY = 3;
constexpr std::uint32_t A_TICK_BETWEEN_THE_STATES = 63;
constexpr std::uint32_t A_TICK_FAR_AFTER_THE_STATES = 600;

/** @brief Where the tick shown lies, by hand: the first state alone puts it
 * 2 assumed intervals of 3 ticks behind; once two states measured an interval
 * of 6 ticks it lies 12 behind the newest state. */
constexpr double ONE_STATE_SHOWN_TICK = 54.0;
constexpr double ONE_TICK_LATER_SHOWN_TICK = 55.0;
constexpr double TWELVE_TICKS_BEHIND_THE_FOURTH_STATE = 66.0;
constexpr double TWELVE_TICKS_BEHIND_THE_FIFTH_STATE = 72.0;
constexpr double TWELVE_TICKS_BEHIND_THE_FAR_STATE = 588.0;
constexpr double SIX_TICKS_AFTER_THE_FIRST_STATE = 66.0;

/** @brief What a test reads when the clock shows nothing: a tick no test
 * expects. */
constexpr double NO_SHOWN_TICK = -1.0e9;

/** @brief The tick the server is at when the first state arrives, 10 ticks
 * after the tick the state carries: it arrives 10 ticks late. */
constexpr double SERVER_TICK_TEN_LATE = 70.0;
/** @brief The tick the server is at when the second state, of tick 63,
 * arrives 4 ticks late. */
constexpr double SERVER_TICK_FOUR_LATE = 67.0;
/** @brief Two assumed intervals of 3 ticks behind the tick of the server,
 * less the worst lateness, 10 ticks: 67 - 10 - 6. */
constexpr double BEHIND_THE_WORST_LATENESS = 51.0;
/** @brief Two intervals of 3 ticks behind the tick of the server, 67, less the
 * lateness of the second state, 4 ticks: 67 - 4 - 6. */
constexpr double BEHIND_THE_USUAL_LATENESS = 57.0;
/** @brief The tick the server is at when the state of tick 60 arrives, 2
 * ticks before the tick the state carries. */
constexpr double SERVER_TICK_BEFORE_THE_STATE = 58.0;
/** @brief Two intervals of 3 ticks behind the tick of the server, 58, with a
 * lateness of 0: 58 - 0 - 6. */
constexpr double BEHIND_THE_CLOCK_ON_TIME = 52.0;
/** @brief A tick of the server of an older game, far from any state. */
constexpr double STALE_SERVER_TICK = 100000.0;
/** @brief The lateness of the states that follow a late one. */
constexpr std::uint32_t USUAL_LATENESS_TICKS = 4;
/** @brief How many states follow the late one: the late one leaves the 32
 * the clock keeps at the 32nd, and the tick shown, 6 ticks short at that
 * moment, takes back a tenth of the gap at each of the 15 that follow, so it
 * is 6 * 0.9^16 = 1.1 ticks short at the last. */
constexpr std::size_t STATES_AFTER_THE_LATE_ONE = 47;
/** @brief Two intervals of 3 ticks, the delay behind the clock of the server
 * once the interval between the states is measured. */
constexpr double TWO_INTERVALS_OF_THREE_TICKS = 6.0;
/** @brief The lateness of the first state of that run. */
constexpr std::uint32_t SPIKE_LATENESS_TICKS = 10;
/** @brief How far from the tick behind the usual lateness the tick shown may
 * be once the late state is forgotten: above the 1.1 ticks it is still short
 * by, and under the 6 ticks it would stay short by if the late state was
 * remembered. */
constexpr double FORGOTTEN_SPIKE_TOLERANCE = 1.5;

/** @brief SIMULATION_TICK_DURATION holds whole nanoseconds, so a frame of
 * "one tick" is 4e-8 of a tick short: the shown tick is checked to a
 * hundred-thousandth of a tick. */
constexpr double SHOWN_TICK_TOLERANCE = 1e-5;

/** @brief An entity with two samples, at ticks 48 and 60: halfway between them
 * is the tick 54 the clock shows after the state of tick 60. */
constexpr PositionSample SAMPLE_BEFORE{.tick = 48,
                                       .position = {.x = 100.0F, .y = 0.0F}};
constexpr PositionSample SAMPLE_AFTER{.tick = 60,
                                      .position = {.x = 220.0F, .y = 60.0F}};
constexpr game::Position HALFWAY_BETWEEN_THE_SAMPLES{.x = 160.0F, .y = 30.0F};
constexpr game::Position WHERE_THE_SERVER_PUT_IT{.x = 5.0F, .y = 7.0F};

/** @brief A Bydo of 240 units per second, which is 4 units per tick, flying
 * along x. */
constexpr float UNITS_PER_TICK = 4.0F;
constexpr std::uint32_t FIRST_SCENARIO_TICK = 60;
constexpr std::size_t STATES_IN_THE_SCENARIO = 200;
constexpr std::size_t FRAMES_BEFORE_THE_CLOCK_SETTLES = 150;
constexpr std::size_t EVERY_TWENTIETH_STATE = 20;
constexpr std::size_t FIRST_OF_TWO_LOST_IN_A_ROW = 100;
constexpr std::size_t SECOND_OF_TWO_LOST_IN_A_ROW = 101;
constexpr std::size_t JITTER_PERIOD = 3;
constexpr std::uint32_t LATE_ARRIVAL_TICKS = 1;

/** @brief A frame of the scenario moves a Bydo by UNITS_PER_TICK; the tick
 * shown may run a little faster while the clock is pulled back, so no step
 * is allowed more than twice that. */
constexpr float LARGEST_SMOOTH_STEP = 2.0F * UNITS_PER_TICK;

/** @brief The step the latest state makes at 10 states per second, 6 ticks
 * apart, and the least a jump of 20 per second is allowed to be to count as
 * a jump. */
constexpr float SMALLEST_JUMP = 12.0F;

}  // namespace rtype::client::testing
