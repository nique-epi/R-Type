#pragma once

#include <cstddef>
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The symbols of the Huffman code: the 256 byte values, then the
 * end-of-body symbol.
 */
constexpr std::size_t HUFFMAN_SYMBOL_COUNT = 257;

/**
 * @brief The symbol that ends a coded body, after the 256 byte values.
 */
constexpr std::size_t END_OF_BODY_SYMBOL = 256;

/**
 * @brief The longest Huffman code in bits, as in DEFLATE (RFC 1951).
 */
constexpr std::size_t MAXIMUM_CODE_LENGTH = 15;

/**
 * @brief The most bytes the body of a datagram holds: the datagram less its
 * packet header. A decoder never produces more.
 */
constexpr std::size_t MAXIMUM_BODY_SIZE =
    MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE;

}  // namespace rtype::network
