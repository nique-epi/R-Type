#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief The ticks of the states the tests rebuild, one after the other, and
 * the tick the server restarts at when a new game starts. */
constexpr std::uint32_t FIRST_TICK = 3;
constexpr std::uint32_t SECOND_TICK = 6;
constexpr std::uint32_t LATE_TICK_OF_A_LONG_GAME = 100;
constexpr std::uint32_t TICK_OF_A_NEW_GAME = 2;

/** @brief How far back the second tick reaches to the first. */
constexpr std::uint8_t THREE_TICKS_BACK = 3;

/** @brief The ids of the entities of the tests. */
constexpr std::uint32_t FIRST_ENTITY_ID = 1;
constexpr std::uint32_t SECOND_ENTITY_ID = 2;
constexpr std::uint32_t THIRD_ENTITY_ID = 3;

/** @brief The abscissas the tests give an entity before and after it moves. */
constexpr float BEFORE_THE_MOVE = 10.0F;
constexpr float AFTER_THE_MOVE = 20.0F;

/** @brief The part counts of the tests: one, two. */
constexpr std::uint8_t ONE_PART = 1;
constexpr std::uint8_t TWO_PARTS = 2;
constexpr std::uint8_t FIRST_PART = 0;
constexpr std::uint8_t SECOND_PART = 1;

/** @brief A part count that disagrees with TWO_PARTS. */
constexpr std::uint8_t THREE_PARTS_COUNT_MISMATCH = 3;

/** @brief The longest reach of a delta, in ticks, one tick past it, and the
 * ticks of the states that fill the history with its first state at tick 1. */
constexpr std::uint8_t LONGEST_REACH = 60;
constexpr std::uint8_t ONE_TICK_PAST_THE_REACH = 61;
constexpr std::uint32_t FIRST_TICK_OF_A_RUN = 1;
constexpr std::uint32_t LAST_TICK_OF_A_FULL_HISTORY = 60;
constexpr std::uint32_t TICK_AFTER_A_FULL_HISTORY = 61;
constexpr std::uint32_t TICK_AFTER_AN_OVERFULL_HISTORY = 62;

}  // namespace rtype::client::testing
