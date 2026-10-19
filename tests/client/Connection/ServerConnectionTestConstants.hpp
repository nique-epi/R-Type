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

/** @brief Another token, which a forged datagram could carry. */
constexpr std::uint64_t OTHER_SESSION_TOKEN = 0x0fedcba987654321;

/** @brief Sequences of the server's datagrams in a session: the acceptance,
 * then two more in the order they were sent. */
constexpr std::uint16_t ACCEPTANCE_SEQUENCE = 0;
constexpr std::uint16_t FIRST_SEQUENCE = 1;
constexpr std::uint16_t SECOND_SEQUENCE = 2;

/** @brief Another player of the waiting room. */
constexpr std::uint8_t OTHER_PLAYER_NUMBER = 1;
constexpr std::string_view OTHER_NICKNAME = "ACE";

/**
 * @brief The repetitions the protocol page gives a session: a ReadyState every
 * 500 ms while the waiting room shows another value, a Disconnection three
 * times, 50 ms apart.
 */
constexpr std::chrono::milliseconds PROTOCOL_READY_STATE_INTERVAL{500};
constexpr int PROTOCOL_DISCONNECTION_COPY_COUNT = 3;
constexpr std::chrono::milliseconds PROTOCOL_DISCONNECTION_COPY_INTERVAL{50};

/**
 * @brief The shortest step of time a test takes.
 */
constexpr std::chrono::nanoseconds ONE_NANOSECOND{1};

}  // namespace rtype::client::testing
