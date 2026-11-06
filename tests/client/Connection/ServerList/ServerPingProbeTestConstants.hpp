#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The two servers of a round, and one the round does not name. */
constexpr std::string_view PARIS_ADDRESS = "203.0.113.7";
constexpr std::string_view LYON_ADDRESS = "203.0.113.8";
constexpr std::string_view STRANGER_ADDRESS = "198.51.100.9";
constexpr std::uint16_t GAME_PORT = 4242;

/** @brief When the round is sent, on the clock of the probe, and when an
 * answer the server held 2 ms arrives 30 ms later: a ping of 28 ms. */
constexpr std::chrono::seconds ROUND_SENT_AT{1};
constexpr std::chrono::milliseconds ANSWER_ARRIVES_AFTER{30};
constexpr std::uint32_t HELD_FOR_MICROSECONDS = 2'000;
constexpr std::chrono::microseconds EXPECTED_PING{28'000};

/** @brief A later answer of the same server, 50 ms after the round. */
constexpr std::chrono::milliseconds SECOND_ANSWER_ARRIVES_AFTER{50};

/** @brief A sentAt the round never sent. */
constexpr std::uint32_t ANOTHER_SENT_AT = 7;

/** @brief What the answers of the servers say of themselves. */
constexpr std::string_view SERVER_NAME = "Paris 1";
constexpr std::string_view SERVER_GAME_VERSION = "0.4.0";

/** @brief The size every ServerInformationRequest has on the wire, so a server
 * answers it: the protocol fixes it at 106 bytes. */
constexpr std::size_t REQUEST_DATAGRAM_SIZE = 106;

/** @brief Bytes that are no datagram of the protocol. */
constexpr std::size_t GARBAGE_SIZE = 5;
constexpr std::byte GARBAGE_BYTE{0xAB};

}  // namespace rtype::client::testing
