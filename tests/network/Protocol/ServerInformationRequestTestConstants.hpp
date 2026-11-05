#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief A sentAt whose four bytes differ, so their order shows on the wire.
 */
constexpr std::uint32_t SOME_SENT_AT = 0x01020304;

/** @brief The size of a datagram holding a request and the zero bytes of its
 * padding, added up by hand from the protocol page: 22 + 3 + 4 + 77, and the
 * 77 bytes of the padding alone. */
constexpr std::size_t REQUEST_DATAGRAM_ON_THE_PROTOCOL_PAGE = 106;
constexpr std::size_t PADDING_ON_THE_PROTOCOL_PAGE = 77;

/** @brief The bytes of sentAt, before the padding. */
constexpr std::size_t SENT_AT_BYTES = 4;

/** @brief A byte of padding other than 0. */
constexpr std::uint8_t NONZERO_PADDING_BYTE = 0x01;

}  // namespace rtype::network::testing
