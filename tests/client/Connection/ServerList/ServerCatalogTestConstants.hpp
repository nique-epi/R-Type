#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The servers of the lists the master answers in the tests. */
constexpr std::int64_t PARIS_ID = 7;
constexpr std::int64_t LYON_ID = 8;
constexpr std::int64_t BREST_ID = 9;
constexpr std::int64_t NANTES_ID = 10;
constexpr std::string_view PARIS_SERVER_ADDRESS = "203.0.113.7";
constexpr std::string_view LYON_SERVER_ADDRESS = "203.0.113.8";
constexpr std::string_view BREST_SERVER_ADDRESS = "203.0.113.9";
constexpr std::string_view NANTES_SERVER_ADDRESS = "203.0.113.10";
constexpr std::uint16_t SERVER_UDP_PORT = 4242;
constexpr std::uint16_t SERVER_MAXIMUM_GAMES = 2;
constexpr std::uint8_t SERVER_PLAYERS_PER_GAME = 4;
/** @brief The players two games of four hold at most. */
constexpr std::uint16_t SERVER_MAXIMUM_PLAYERS = 8;

/** @brief A version of the protocol the client does not speak. */
constexpr std::uint8_t OTHER_PROTOCOL_VERSION = 2;

/** @brief Pings the servers answer with, in the order they are listed. */
constexpr std::chrono::microseconds LOW_PING{24'000};
constexpr std::chrono::microseconds HIGH_PING{88'000};

/**
 * @brief A frame of the network stage, and the time after which a round
 * closes, PING_GIVE_UP_TIME, written from the requirement: 2 s. The round
 * starts in the frame that takes the listing, so it is ONE_FRAME old after
 * it: 16 ms then 1980 ms make 1996 ms, short of 2 s.
 */
constexpr std::chrono::milliseconds ONE_FRAME{16};
constexpr std::chrono::seconds ROUND_GIVE_UP{2};
constexpr std::chrono::milliseconds JUST_BEFORE_GIVE_UP{1'980};

}  // namespace rtype::client::testing
