#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "HandshakeConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief Address the test players send from, and one port per player: one
 * more than a game holds.
 */
constexpr std::string_view PLAYER_ADDRESS = "192.0.2.20";
constexpr std::array<std::uint16_t, network::MAXIMUM_PLAYERS + 1>
    FIVE_PLAYER_PORTS{40001, 40002, 40003, 40004, 40005};

/**
 * @brief A nickname the protocol accepts.
 */
constexpr std::string_view PLAYER_NICKNAME = "Nolan";

/**
 * @brief The shortest nickname the protocol accepts: one byte.
 */
constexpr std::string_view SHORTEST_NICKNAME = "A";

/**
 * @brief The start of the nickname nicknameOf() gives each player, followed by
 * the number of the player: Pilot0, Pilot1…
 */
constexpr std::string_view NICKNAME_PREFIX = "Pilot";

/**
 * @brief Two session tokens a roster may be given.
 */
constexpr std::uint64_t SESSION_TOKEN = 0x0123456789abcdef;
constexpr std::uint64_t OTHER_SESSION_TOKEN = 0xfedcba9876543210;

/**
 * @brief When the test requests arrive, and a time at which a cookie issued
 * then has expired: two cookie windows later.
 */
constexpr std::chrono::nanoseconds REQUEST_TIME = std::chrono::seconds{100};
constexpr std::chrono::nanoseconds COOKIE_EXPIRED_TIME =
    REQUEST_TIME + (2 * network::COOKIE_WINDOW_DURATION);

/**
 * @brief Times of the requests a connected player sends again, from
 * REQUEST_TIME, its first acceptance: 250 ms is half the 500 ms between two
 * requests of the protocol page, the least time between two acceptances to
 * one player; the steps before it, and the next repeat 250 ms later.
 */
constexpr std::chrono::nanoseconds A_TENTH_OF_A_SECOND_LATER =
    REQUEST_TIME + std::chrono::milliseconds{100};
constexpr std::chrono::nanoseconds A_FIFTH_OF_A_SECOND_LATER =
    REQUEST_TIME + std::chrono::milliseconds{200};
constexpr std::chrono::nanoseconds JUST_BEFORE_THE_REPEAT =
    REQUEST_TIME + std::chrono::milliseconds{249};
constexpr std::chrono::nanoseconds ACCEPTANCE_REPEAT_TIME =
    REQUEST_TIME + std::chrono::milliseconds{250};
constexpr std::chrono::nanoseconds JUST_AFTER_THE_REPEAT =
    REQUEST_TIME + std::chrono::milliseconds{300};
constexpr std::chrono::nanoseconds NEXT_ACCEPTANCE_REPEAT_TIME =
    REQUEST_TIME + std::chrono::milliseconds{500};

/** @brief The name of the entity type of the sample acceptances: the ship. */
constexpr std::string_view SAMPLE_ENTITY_TYPE_NAME = "ship";

/**
 * @brief A protocol id and a version this server does not speak.
 */
constexpr std::uint32_t OTHER_PROTOCOL_ID = network::PROTOCOL_ID + 1;
constexpr std::uint8_t NEXT_PROTOCOL_VERSION = network::PROTOCOL_VERSION + 1;

/**
 * @brief The 26 bytes of the protocol page below which the server answers no
 * datagram of another version: the size of its ConnectionRefused.
 */
constexpr std::size_t REFUSAL_DATAGRAM_SIZE = 26;

/**
 * @brief A flags byte with a flag version 4 of the protocol does not know: it
 * knows only 0x01 and 0x02.
 */
constexpr std::uint8_t UNKNOWN_FLAG = 0x04;

/**
 * @brief Words of every log line about a datagram the server drops, which tell
 * those lines apart from the others, and how many datagrams a test has dropped
 * at once to check that they share one line.
 */
constexpr std::string_view DROPPED_DATAGRAM_LINE_TEXT = "dropped a datagram";
constexpr std::size_t DROPPED_TOGETHER_COUNT = 3;

/** @brief The network id of a ship in the tests that only need one. */
constexpr std::uint32_t SOME_SHIP_ID = 7;

}  // namespace rtype::server::testing
