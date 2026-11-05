#pragma once

#include <array>
#include <cstdint>
#include "CompressionConstants.hpp"
#include "HuffmanCode.hpp"

namespace rtype::network {

/**
 * @brief A canonical Huffman code (RFC 1951, section 3.2.2), what the encoder
 * writes and the decoder reads. Codes of one length are consecutive numbers,
 * handed to the symbols in increasing order, so the lengths alone give back
 * every code.
 */
struct HuffmanTable {
  /** The code of each symbol. */
  std::array<HuffmanCode, HUFFMAN_SYMBOL_COUNT> codes{};
  /** How many symbols have a code of each length; index 0 is always 0. */
  std::array<std::uint16_t, MAXIMUM_CODE_LENGTH + 1> countByLength{};
  /** The symbols ordered by code length, then by symbol number. */
  std::array<std::uint16_t, HUFFMAN_SYMBOL_COUNT> symbolsByCode{};
};

/**
 * @brief Builds the code of the given lengths.
 *
 * @pre Every length is from 1 to MAXIMUM_CODE_LENGTH and together they form a
 * complete prefix code, as the table of train_table.py does.
 */
[[nodiscard]] HuffmanTable buildHuffmanTable(
    const std::array<std::uint8_t, HUFFMAN_SYMBOL_COUNT>& lengths);

/**
 * @returns The code of the datagrams the server sends, built once from
 * SERVER_TO_CLIENT_CODE_LENGTHS. Safe to call from any thread.
 */
[[nodiscard]] const HuffmanTable& serverToClientTable();

}  // namespace rtype::network
