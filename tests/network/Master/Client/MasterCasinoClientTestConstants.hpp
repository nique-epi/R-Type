#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

/** @brief The room the tests of the casino client play in, and a round of its
 * wheel. */
constexpr std::uint8_t CASINO_TEST_ROOM = 2;
constexpr rtype::network::RoomRound CASINO_TEST_ROUND{.room = CASINO_TEST_ROOM,
                                                      .round = 815};

/** @brief Its name, its seats, and the players seated in it. */
constexpr std::string_view CASINO_TEST_ROOM_NAME = "Nebula Lounge";
constexpr std::uint8_t CASINO_TEST_SEATS = 8;
constexpr std::uint8_t CASINO_TEST_SEATED = 1;

/** @brief The routes of the casino for that room and round, as
 * FakeAccountMaster names them. */
constexpr std::string_view LIST_CASINO_ROOMS = "GET /api/casino/rooms";
constexpr std::string_view READ_TEST_ROOM = "GET /api/casino/rooms/2";
constexpr std::string_view TAKE_TEST_SEAT = "PUT /api/casino/rooms/2/seats/me";
constexpr std::string_view LEAVE_TEST_SEAT =
    "DELETE /api/casino/rooms/2/seats/me";
constexpr std::string_view PLACE_TEST_BETS =
    "PUT /api/casino/rooms/2/rounds/815/bets/me";
constexpr std::string_view READ_TEST_BETS =
    "GET /api/casino/rooms/2/rounds/815/bets/me";

/** @brief What the master answers a seat left: no content. */
constexpr int SEAT_LEFT_STATUS = 204;

/** @brief When the bets of the round close, the wheel stops and the round
 * ends, and the clock of the master as it answers, in milliseconds since
 * 1970. */
constexpr std::chrono::milliseconds TEST_ROUND_CLOSES_AT{1'790'000'020'000};
constexpr std::chrono::milliseconds TEST_ROUND_RESULT_AT{1'790'000'026'000};
constexpr std::chrono::milliseconds TEST_ROUND_ENDS_AT{1'790'000'031'000};
constexpr std::chrono::milliseconds TEST_MASTER_TIME{1'790'000'012'000};

/** @brief Ten chips on 17, what the wallet holds once they are staked, and
 * what 17 drawn paid them. */
constexpr rtype::network::RouletteBet TEN_ON_17{
    .kind = rtype::network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 10};
constexpr std::uint32_t CHIPS_LEFT_AFTER_TEN = 490;
constexpr std::uint8_t SEVENTEEN = 17;
constexpr std::uint32_t TEN_ON_17_PAID = 360;

/** @brief The answer of the master to bets on a closed round. */
constexpr std::string_view BETS_CLOSED_PROBLEM =
    R"({"title":"Conflict","status":409,"detail":"the bets of that round are closed"})";
