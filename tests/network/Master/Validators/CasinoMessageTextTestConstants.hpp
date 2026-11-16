#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief A room of the tests: its number, name, seats, players seated and
 * round. */
constexpr std::uint8_t TEST_ROOM_ID = 2;
constexpr std::string_view TEST_ROOM_NAME = "Nebula Lounge";
constexpr std::uint8_t TEST_ROOM_SEATS = 8;
constexpr std::uint8_t TEST_ROOM_SEATED = 3;
constexpr std::uint64_t TEST_ROUND = 59'000'123;

/** @brief The dates of the phases of the round of the tests, in milliseconds
 * since 1970, and the clock of the master. */
constexpr std::chrono::milliseconds TEST_CLOSES_AT{1'770'000'020'000};
constexpr std::chrono::milliseconds TEST_RESULT_AT{1'770'000'026'000};
constexpr std::chrono::milliseconds TEST_ENDS_AT{1'770'000'031'000};
constexpr std::chrono::milliseconds TEST_SERVER_TIME{1'770'000'012'345};

/** @brief The number the round of the tests drew, and the numbers before. */
constexpr std::uint8_t TEST_WINNING_NUMBER = 17;
constexpr std::uint8_t TEST_EARLIER_NUMBER = 0;
constexpr std::uint8_t TEST_EARLIEST_NUMBER = 36;

/** @brief A player of the room of the tests: its stake and its payout. */
constexpr std::string_view TEST_CASINO_NICKNAME = "ACE";
constexpr std::uint32_t TEST_STAKE = 35;
constexpr std::uint32_t TEST_PAYOUT = 360;
constexpr std::uint32_t TEST_CHIPS_LEFT = 465;

/** @brief Two bets of the tests: ten chips on 17, and 25 on red. */
constexpr std::uint8_t STRAIGHT_NUMBER = 17;
constexpr std::uint32_t STRAIGHT_AMOUNT = 10;
constexpr std::uint32_t RED_AMOUNT = 25;

/** @brief Bodies of bets: one straight bet written by a client, a kind no
 * roulette has, and a list longer than the master reads. */
constexpr std::string_view ONE_STRAIGHT_BODY =
    R"({"bets":[{"kind":"straight","firstNumber":17,"amount":10}]})";
constexpr std::string_view UNKNOWN_KIND_BODY =
    R"({"bets":[{"kind":"triple","firstNumber":1,"amount":10}]})";
constexpr std::string_view UNKNOWN_KIND_POINTER = "/bets/0/kind";
constexpr std::size_t TOO_LONG_BET_LIST = 65;

/** @brief A state whose number is still unknown: written null. */
constexpr std::string_view NULL_WINNING_NUMBER = R"("winningNumber":null)";

/** @brief How the phase of the result is written, and a phase no round has. */
constexpr std::string_view RESULT_PHASE_TEXT = "result";
constexpr std::string_view NO_PHASE_TEXT = "over";

}  // namespace rtype::network::testing
