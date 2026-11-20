#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client::testing {

/** @brief The local player of the ballot tests, its nickname, and the map it
 * votes for, the second. */
constexpr std::uint8_t BALLOT_PLAYER_NUMBER = 2;
constexpr const char* BALLOT_PLAYER_NICKNAME = "ACE";
constexpr std::uint8_t BALLOT_MAP = 2;

/** @brief Just short of the 500 ms after which a vote the room does not show
 * goes out again, and the millisecond that reaches it. */
constexpr std::chrono::milliseconds JUST_SHORT_OF_A_RESEND{499};
constexpr std::chrono::milliseconds LAST_MILLISECOND{1};

}  // namespace rtype::client::testing
