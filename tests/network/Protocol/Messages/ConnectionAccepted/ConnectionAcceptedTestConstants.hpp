#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief The number of the first player. */
constexpr std::uint8_t FIRST_PLAYER_BYTE = 1;

/** @brief The size and the port an acceptance of the tests announces: a size
 * and a port whose four and two bytes all differ, so the order of the bytes
 * shows. */
constexpr std::uint32_t SOME_PACK_SIZE = 0x00010203;
constexpr std::uint16_t SOME_PACK_PORT = 0x1092;

/** @brief The bytes of the size and the port above, in network byte order. */
constexpr std::uint8_t SIZE_FIRST_BYTE = 0x00;
constexpr std::uint8_t SIZE_SECOND_BYTE = 0x01;
constexpr std::uint8_t SIZE_THIRD_BYTE = 0x02;
constexpr std::uint8_t SIZE_FOURTH_BYTE = 0x03;
constexpr std::uint8_t PORT_FIRST_BYTE = 0x10;
constexpr std::uint8_t PORT_SECOND_BYTE = 0x92;

/** @brief The payload of an acceptance as the protocol page states it: the
 * player number (1), the fingerprint (32), the size (4) and the port (2). */
constexpr std::size_t ACCEPTANCE_PAYLOAD_BYTES = 39;

/** @brief The largest pack an acceptance may announce, 8 MiB, as the
 * protocol page states it, and one byte more. */
constexpr std::uint32_t LARGEST_PACK_SIZE = 8U * 1024U * 1024U;
constexpr std::uint32_t ONE_BYTE_TOO_LARGE = LARGEST_PACK_SIZE + 1;

/** @brief Where the last byte of the size lies in the payload, after the
 * player number (1), the fingerprint (32) and three bytes of the size; and
 * the bit that makes the largest size one byte too large there. */
constexpr std::size_t LAST_SIZE_BYTE = 36;
constexpr std::byte LOWEST_BIT{1};

/** @brief A byte left after the port. */
constexpr std::byte TRAILING_BYTE{0};

}  // namespace rtype::network::testing
