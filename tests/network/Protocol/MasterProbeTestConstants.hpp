#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief A secret and a token of a probe. */
constexpr std::uint64_t PROBE_SECRET = 0x1234'5678'9ABC'DEF0;
constexpr std::uint64_t PROBE_TOKEN = 0x0FED'CBA9'8765'4321;

/** @brief The bytes of the datagram of a probe on the protocol page: 22 of
 * header, 3 of prefix, 8 of secret and 8 of token. */
constexpr std::size_t PROBE_DATAGRAM_BYTES = 41;

/** @brief Bytes of a payload one byte short, and one byte too long. */
constexpr std::size_t PROBE_PAYLOAD_ONE_SHORT = 15;
constexpr std::size_t PROBE_PAYLOAD_ONE_LONG = 17;

}  // namespace rtype::network::testing
