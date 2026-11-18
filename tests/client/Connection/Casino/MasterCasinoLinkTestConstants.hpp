#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "CasinoSeat.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief How long a test waits for the thread of the casino link to answer:
 * far more than a call to the loopback takes. */
constexpr std::chrono::seconds CASINO_LINK_DEADLINE{10};

/** @brief The token the tests play with. */
constexpr std::string_view CASINO_LINK_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief The room of the tests, a seat in it and the way up, and a round of
 * its wheel. */
constexpr std::uint8_t LINK_ROOM = 3;
constexpr CasinoSeat SEATED_IN_LINK_ROOM{.room = LINK_ROOM, .seated = true};
constexpr CasinoSeat UP_FROM_LINK_ROOM{.room = LINK_ROOM, .seated = false};
constexpr network::RoomRound LINK_ROUND{.room = LINK_ROOM, .round = 1'200};

/** @brief The routes of that room and round, as FakeAccountMaster names
 * them. */
constexpr std::string_view LINK_ROOM_ROUTE = "GET /api/casino/rooms/3";
constexpr std::string_view LINK_SEAT_ROUTE = "PUT /api/casino/rooms/3/seats/me";
constexpr std::string_view LINK_LEAVE_ROUTE =
    "DELETE /api/casino/rooms/3/seats/me";
constexpr std::string_view LINK_BETS_ROUTE =
    "PUT /api/casino/rooms/3/rounds/1200/bets/me";

/** @brief The statuses the master answers with. */
constexpr int CASINO_OK_STATUS = 200;
constexpr int CASINO_NO_CONTENT_STATUS = 204;
constexpr int CASINO_CONFLICT_STATUS = 409;
constexpr int CASINO_UNPROCESSABLE_STATUS = 422;

/** @brief The problem documents of the refusals the client tells apart. */
constexpr std::string_view BETS_CLOSED_ANSWER =
    R"({"detail":"the bets of that round are closed"})";
constexpr std::string_view NOT_SEATED_ANSWER =
    R"({"detail":"take a seat in the room before betting"})";
constexpr std::string_view NOT_ENOUGH_CHIPS_ANSWER =
    R"({"detail":"the bets hold more chips than the wallet"})";
constexpr std::string_view ROOM_FULL_ANSWER =
    R"({"detail":"every seat of the room is taken"})";

/** @brief Ten chips on red. */
constexpr network::RouletteBet TEN_ON_RED{
    .kind = network::RouletteBetKind::Red, .firstNumber = 0, .amount = 10};

/** @brief When the round of the tests closes, stops and ends, and the clock
 * of the master as it answers, in milliseconds since 1970. */
constexpr std::chrono::milliseconds LINK_CLOSES_AT{1'790'000'020'000};
constexpr std::chrono::milliseconds LINK_RESULT_AT{1'790'000'026'000};
constexpr std::chrono::milliseconds LINK_ENDS_AT{1'790'000'031'000};
constexpr std::chrono::milliseconds LINK_MASTER_TIME{1'790'000'012'000};

/** @brief The name of the room and its seats. */
constexpr std::string_view LINK_ROOM_NAME = "Wave Cannon Club";
constexpr std::uint8_t LINK_ROOM_SEATS = 8;

}  // namespace rtype::client::testing
