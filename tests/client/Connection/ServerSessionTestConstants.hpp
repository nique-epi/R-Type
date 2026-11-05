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

/** @brief The score of the accepted player when the server ends the game. */
constexpr std::uint32_t PILOT_FINAL_SCORE = 1200;

/**
 * @brief A session token a server could draw, and a version this client does
 * not speak.
 */
constexpr std::uint64_t SESSION_TOKEN = 0x0123456789abcdef;
constexpr std::uint8_t NEXT_PROTOCOL_VERSION = 4;

/** @brief The version a server of this protocol writes in its headers. */
constexpr std::uint8_t SERVER_VERSION = 3;

/**
 * @brief Another session token, as a datagram forging the address of the
 * server would carry.
 */
constexpr std::uint64_t OTHER_SESSION_TOKEN = 0xfedcba9876543210;

/**
 * @brief The delays the protocol page gives an attempt: a request every
 * 500 ms while the server does not answer, and 5 s before giving up.
 */
constexpr std::chrono::milliseconds PROTOCOL_REQUEST_INTERVAL{500};
constexpr std::chrono::seconds PROTOCOL_ATTEMPT_TIMEOUT{5};

/** @brief Sequences of the server's datagrams in a session: the acceptance,
 * then four more in the order they were sent. */
constexpr std::uint16_t ACCEPTANCE_SEQUENCE = 0;
constexpr std::uint16_t FIRST_SEQUENCE = 1;
constexpr std::uint16_t SECOND_SEQUENCE = 2;
constexpr std::uint16_t THIRD_SEQUENCE = 3;
constexpr std::uint16_t FOURTH_SEQUENCE = 4;

/** @brief The acknowledgement bits after the acceptance (0), FIRST_SEQUENCE and
 * THIRD_SEQUENCE, SECOND_SEQUENCE being lost: bit 0 (2) clear, bit 1 (1) and
 * bit 2 (0) set. */
constexpr std::uint32_t BITS_OF_THE_FIRST_AND_THE_ACCEPTANCE = 0b110;

/** @brief Number of the GameEvent the tests send, the same in every copy. */
constexpr std::uint16_t EVENT_NUMBER = 42;

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
 * @brief What the protocol page says of the inputs: the first one is numbered
 * 1, and up is bit 0 of the actions.
 */
constexpr std::uint32_t PROTOCOL_FIRST_INPUT_NUMBER = 1;
constexpr std::uint8_t PROTOCOL_UP_ACTION_BIT = 0b00001;

/**
 * @brief The 500 ms of the protocol page between two Heartbeats of a side of
 * a session, whatever else it sends.
 */
constexpr std::chrono::milliseconds PROTOCOL_HEARTBEAT_INTERVAL{500};

/**
 * @brief How long the protocol page lets a server stay silent: after 5 s
 * without a datagram of the server, the client takes it for gone.
 */
constexpr std::chrono::seconds PROTOCOL_SESSION_TIMEOUT{5};

/**
 * @brief The 15 s of the protocol page during which the server keeps the seat
 * of a silent player, and the 20 s of silence after which the client takes the
 * server for gone: the timeout and the grace.
 */
constexpr std::chrono::seconds PROTOCOL_RECONNECTION_GRACE{15};
constexpr std::chrono::seconds PROTOCOL_SILENCE_LIMIT{20};

/**
 * @brief The words the client logs when it gives the server of the tests up
 * after the 20 s of the protocol page, the timeout and the grace.
 */
constexpr std::string_view LOST_SERVER_LINE_TEXT =
    "no answer from 192.0.2.30 port 4242 for 20 s";

}  // namespace rtype::client::testing
