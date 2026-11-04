#pragma once

#include <cstdint>
#include "ProtocolConstants.hpp"

namespace rtype::server::testing {

/** @brief The ticks of the states the tests send, one after the other. */
constexpr std::uint32_t FIRST_SENT_TICK = 10;
constexpr std::uint32_t SECOND_SENT_TICK = 11;
constexpr std::uint32_t THIRD_SENT_TICK = 12;

/** @brief The sequences of the datagrams of two parts, and of a third state's
 * single part. */
constexpr std::uint16_t FIRST_PART_SEQUENCE = 5;
constexpr std::uint16_t SECOND_PART_SEQUENCE = 6;
constexpr std::uint16_t THIRD_STATE_SEQUENCE = 7;

/** @brief A header acknowledges the datagram one before its newest with bit 0
 * of acknowledgementBits. */
constexpr std::uint32_t ONLY_THE_DATAGRAM_JUST_BEFORE = 1;
constexpr std::uint32_t NO_EARLIER_DATAGRAM = 0;

/** @brief The last sequence, and the first one after it. */
constexpr std::uint16_t LAST_SEQUENCE_BEFORE_WRAPPING = 65535;
constexpr std::uint16_t FIRST_SEQUENCE_AFTER_WRAPPING = 0;

/** @brief A tick of a game that started over, below the ticks sent before. */
constexpr std::uint32_t TICK_OF_A_NEW_GAME = 3;

/** @brief The tick of the first state of a run longer than the history. */
constexpr std::uint32_t TICK_OF_A_RUN_START = 100;

/** @brief One tick past the furthest a delta reaches back. */
constexpr std::uint32_t ONE_TICK_BEYOND_THE_REACH = 1;

}  // namespace rtype::server::testing
