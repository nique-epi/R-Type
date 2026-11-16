#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "CasinoSeat.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief The account the player plays with, and its token. */
constexpr std::uint64_t CHANNEL_ACCOUNT_ID = 7;
constexpr std::string_view CHANNEL_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief The room the player enters, a seat in it and the way up, and the
 * round of its wheel in progress. */
constexpr std::uint8_t CHANNEL_ROOM = 4;
constexpr CasinoSeat SEATED_IN_CHANNEL_ROOM{.room = CHANNEL_ROOM,
                                            .seated = true};
constexpr CasinoSeat UP_FROM_CHANNEL_ROOM{.room = CHANNEL_ROOM,
                                          .seated = false};
constexpr std::uint64_t CHANNEL_ROUND = 600;

/** @brief The clock of the master when it answered, when the bets close, the
 * wheel stops and the round ends, and a time before it answered. */
constexpr std::chrono::milliseconds CHANNEL_MASTER_TIME{1'790'000'000'000};
constexpr std::chrono::milliseconds CHANNEL_CLOSES_AT{1'790'000'012'000};
constexpr std::chrono::milliseconds CHANNEL_RESULT_AT{1'790'000'018'000};
constexpr std::chrono::milliseconds CHANNEL_ENDS_AT{1'790'000'023'000};
constexpr std::chrono::milliseconds EARLIER_MASTER_TIME{1'789'999'999'000};

/** @brief Times on the clock of the client: a frame, the wait between two
 * reads of the room, a little less, the renewal of a seat and a little less,
 * and enough to close the bets. */
constexpr std::chrono::microseconds CHANNEL_FRAME{16'667};
constexpr std::chrono::milliseconds ROOM_READ_AGAIN_AFTER{500};
constexpr std::chrono::milliseconds JUST_BEFORE_THE_READ{480};
constexpr std::chrono::seconds SEAT_RENEWED_AFTER{10};
constexpr std::chrono::milliseconds JUST_BEFORE_THE_RENEWAL{9'900};
constexpr std::chrono::seconds THREE_SECONDS{3};
constexpr std::chrono::seconds PAST_THE_CLOSE{13};

/** @brief Ten chips on black, what the wallet keeps once staked, the number
 * drawn and what it paid. */
constexpr network::RouletteBet TEN_ON_BLACK{
    .kind = network::RouletteBetKind::Black, .firstNumber = 0, .amount = 10};
constexpr std::uint32_t CHANNEL_CHIPS_LEFT = 90;
constexpr std::uint8_t CHANNEL_DRAWN = 13;
constexpr std::uint32_t TEN_ON_BLACK_PAID = 20;

}  // namespace rtype::client::testing
