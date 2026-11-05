#pragma once

#include <array>
#include <cstdint>
#include "CompressionConstants.hpp"

namespace rtype::network {

/**
 * @brief The bit length of the Huffman code of each symbol: the 256 byte
 * values, then the end-of-body symbol. Produced by
 * tests/tools/compression/train_table.py from 29257 datagram bodies the
 * server sent with a session token in real games; never edited by hand.
 * Both programs hold the same table, so changing it changes the protocol
 * version.
 */
constexpr std::array<std::uint8_t, HUFFMAN_SYMBOL_COUNT>
    SERVER_TO_CLIENT_CODE_LENGTHS{
        1,  4,  4,  5,  6,  7,  7,  8,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,
        8,  9,  8,  9,  8,  8,  8,  8,  8,  8,  8,  7,  9,  9,  8,  9,  9,  9,
        9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  10,
        9,  9,  9,  9,  9,  9,  9,  10, 11, 11, 9,  11, 11, 10, 11, 11, 11, 11,
        11, 11, 10, 11, 11, 9,  10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11,
        9,  10, 11, 11, 11, 11, 9,  11, 11, 11, 8,  10, 11, 11, 11, 10, 11, 11,
        10, 10, 11, 11, 10, 11, 10, 11, 11, 11, 11, 11, 11, 11, 10, 11, 10, 11,
        10, 10, 8,  10, 11, 11, 11, 10, 10, 11, 11, 11, 11, 11, 11, 11, 10, 11,
        10, 11, 11, 11, 11, 10, 12, 11, 11, 11, 11, 11, 11, 10, 11, 12, 9,  12,
        9,  11, 11, 10, 11, 10, 11, 10, 11, 11, 11, 11, 11, 10, 10, 11, 10, 11,
        11, 9,  11, 11, 10, 11, 10, 10, 10, 11, 11, 11, 8,  10, 10, 11, 10, 10,
        9,  11, 9,  10, 10, 11, 11, 11, 11, 10, 10, 11, 11, 11, 11, 11, 11, 10,
        12, 11, 11, 10, 11, 12, 11, 11, 9,  11, 11, 11, 11, 11, 11, 12, 11, 11,
        11, 11, 11, 11, 11, 12, 10, 11, 11, 11, 11, 11, 12, 11, 11, 11, 11, 10,
        11, 11, 11, 11, 7};

}  // namespace rtype::network
