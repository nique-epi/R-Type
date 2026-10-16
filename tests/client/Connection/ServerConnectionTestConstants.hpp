#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/**
 * @brief The server a test connects to, and another sender on the same
 * address.
 */
constexpr std::string_view SERVER_ADDRESS = "192.0.2.30";
constexpr std::uint16_t SERVER_PORT = 4242;
constexpr std::uint16_t OTHER_SENDER_PORT = 4243;

/**
 * @brief The nickname the tests connect with.
 */
constexpr std::string_view NICKNAME = "Nolan";

/**
 * @brief A cookie a server could hand out, and the number it accepts a
 * player as.
 */
constexpr std::uint64_t CHALLENGE_COOKIE = 0x1122334455667788;
constexpr std::uint8_t ACCEPTED_PLAYER_NUMBER = 3;

/**
 * @brief A session token a server could draw, and a version this client does
 * not speak.
 */
constexpr std::uint64_t SESSION_TOKEN = 0x0123456789abcdef;
constexpr std::uint8_t NEXT_PROTOCOL_VERSION = 1;

/**
 * @brief The shortest step of time a test takes.
 */
constexpr std::chrono::nanoseconds ONE_NANOSECOND{1};

}  // namespace rtype::client::testing
