#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

/** @brief The values of the protocol page only these tests read: the bits of
 * acknowledgementBits, the lifetime of a guaranteed message and the version;
 * the flags and the other delays are in NetworkTestConstants.hpp. */
constexpr std::size_t PROTOCOL_ACKNOWLEDGEMENT_BIT_COUNT = 32;
constexpr std::chrono::seconds PROTOCOL_MESSAGE_LIFETIME{1};
constexpr std::uint8_t PROTOCOL_PAGE_VERSION = 4;

/** @brief A first sequence of a session, any one. */
constexpr std::uint16_t FIRST_RECEIVED = 500;

/** @brief 10, 11, then 13: 12 is lost. After them, bit 0 (12) is clear, bit 1
 * (11) and bit 2 (10) are set. */
constexpr std::uint16_t OLDEST_RECEIVED = 10;
constexpr std::uint16_t RECEIVED_BEFORE_THE_LOSS = 11;
constexpr std::uint16_t LOST_SEQUENCE = 12;
constexpr std::uint16_t NEWEST_AFTER_THE_LOSS = 13;
constexpr std::uint32_t BITS_OF_ELEVEN_AND_TEN = 0b110;

/** @brief 0, then 32 or 33 after it: 32 is the farthest a bit names. */
constexpr std::uint16_t ZERO_SEQUENCE = 0;
constexpr std::uint16_t THIRTY_TWO_LATER = 32;
constexpr std::uint16_t THIRTY_THREE_LATER = 33;
/** @brief 65 after 0: the bits are moved in 64 bits, so 65 is the first
 * distance that would move them past that width without the bound on 32. */
constexpr std::uint16_t SIXTY_FIVE_LATER = 65;
constexpr std::uint16_t LAST_SEQUENCE = 65535;

/** @brief Bits of acknowledgementBits the tests write or expect. */
constexpr std::uint32_t NO_BIT = 0;
constexpr std::uint32_t ONLY_BIT_ZERO = 0x00000001;
constexpr std::uint32_t ONLY_BIT_THIRTY_ONE = 0x80000000;
constexpr std::uint32_t EVERY_BIT = 0xFFFFFFFF;

/** @brief The datagrams a message goes in, and the one just after the
 * first. */
constexpr std::uint16_t FIRST_SEND = 5;
constexpr std::uint16_t SEND_AFTER_THE_FIRST = 6;
constexpr std::uint16_t SECOND_SEND = 9;

/** @brief The numbers two messages of the tests carry. */
constexpr std::uint16_t FIRST_MESSAGE = 1;
constexpr std::uint16_t SECOND_MESSAGE = 2;

/** @brief Payload sizes: one byte more than the largest one message carries
 * alone (DATAGRAM_PAYLOAD_LIMIT), and two of 600 never share a datagram
 * (22 + 2 * 603 = 1228). */
constexpr std::size_t PAYLOAD_ONE_BYTE_TOO_LARGE = DATAGRAM_PAYLOAD_LIMIT + 1;
constexpr std::size_t HALF_DATAGRAM_PAYLOAD_SIZE = 600;

/** @brief The demonstration of the issue: one datagram in five lost each way,
 * from fixed seeds; a step of 10 ms, so a message is sent at most 10 times, at
 * steps 0, 10, ..., 90, and dropped at step 100. */
constexpr double ONE_LOSS_IN_FIVE = 0.2;
constexpr std::uint32_t TOWARD_RECEIVER_SEED = 2026;
constexpr std::uint32_t TOWARD_SENDER_SEED = 1105;
constexpr std::chrono::milliseconds DEMONSTRATION_STEP{10};
constexpr std::size_t STEPS_OF_A_LIFETIME = 100;
constexpr std::size_t DEMONSTRATION_MESSAGE_COUNT = 200;
constexpr std::size_t DEMONSTRATION_STEPS =
    DEMONSTRATION_MESSAGE_COUNT + STEPS_OF_A_LIFETIME;

}  // namespace rtype::network::testing
