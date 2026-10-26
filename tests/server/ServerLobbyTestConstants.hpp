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
 * @brief A flags byte with one flag set; version 0 of the protocol sets none.
 */
constexpr std::uint8_t ONE_FLAG_SET = 0x01;

/**
 * @brief Words of every log line about a datagram the server drops, which tell
 * those lines apart from the others, and how many datagrams a test has dropped
 * at once to check that they share one line.
 */
constexpr std::string_view DROPPED_DATAGRAM_LINE_TEXT = "dropped a datagram";
constexpr std::size_t DROPPED_TOGETHER_COUNT = 3;

}  // namespace rtype::server::testing
