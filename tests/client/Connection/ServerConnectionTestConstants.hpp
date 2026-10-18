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
 * @brief The delays the protocol page gives an attempt: a request every
 * 500 ms while the server does not answer, and 5 s before giving up.
 */
constexpr std::chrono::milliseconds PROTOCOL_REQUEST_INTERVAL{500};
constexpr std::chrono::seconds PROTOCOL_ATTEMPT_TIMEOUT{5};

/**
 * @brief What the protocol page says of the inputs: the first one is numbered
 * 1, and up is bit 0 of the actions.
 */
constexpr std::uint32_t PROTOCOL_FIRST_INPUT_NUMBER = 1;
constexpr std::uint8_t PROTOCOL_UP_ACTION_BIT = 0b00001;

/**
 * @brief The shortest step of time a test takes.
 */
constexpr std::chrono::nanoseconds ONE_NANOSECOND{1};

}  // namespace rtype::client::testing
