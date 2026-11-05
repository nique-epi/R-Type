#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "CompressionConstants.hpp"
#include "HuffmanCode.hpp"
#include "HuffmanCodecTestConstants.hpp"
#include "HuffmanTable.hpp"
#include "HuffmanTableConstants.hpp"

using rtype::network::buildHuffmanTable;
using rtype::network::HUFFMAN_SYMBOL_COUNT;
using rtype::network::HuffmanCode;
using rtype::network::HuffmanTable;
using rtype::network::MAXIMUM_CODE_LENGTH;
using rtype::network::SERVER_TO_CLIENT_CODE_LENGTHS;
using rtype::network::serverToClientTable;
using rtype::network::testing::COMPLETE_CODE_WEIGHT;

/**
 * Given the code lengths the server ships
 * When the weight 2^(longest length - length) of every symbol is added up
 * Then the sum is exactly that of a complete prefix code, so no code is unused
 */
TEST(HuffmanTable, ShippedLengthsFormACompleteCode) {
  std::uint32_t weight = 0;
  for (const std::uint8_t length : SERVER_TO_CLIENT_CODE_LENGTHS) {
    weight += 1U << (MAXIMUM_CODE_LENGTH - length);
  }

  EXPECT_EQ(weight, COMPLETE_CODE_WEIGHT);
}

/**
 * Given the code lengths the server ships
 * When each is read
 * Then every symbol has a code of 1 to MAXIMUM_CODE_LENGTH bits, so any body
 *      can be coded
 */
TEST(HuffmanTable, EverySymbolHasACodeOfAcceptableLength) {
  for (const std::uint8_t length : SERVER_TO_CLIENT_CODE_LENGTHS) {
    EXPECT_GE(length, 1U);
    EXPECT_LE(length, MAXIMUM_CODE_LENGTH);
  }
}

/**
 * Given the table built from the shipped lengths
 * When the codes of two different symbols are compared
 * Then the shorter one is never the beginning of the longer one
 */
TEST(HuffmanTable, NoCodeIsTheBeginningOfAnother) {
  const HuffmanTable& table = serverToClientTable();

  for (std::size_t first = 0; first < HUFFMAN_SYMBOL_COUNT; ++first) {
    for (std::size_t second = 0; second < HUFFMAN_SYMBOL_COUNT; ++second) {
      const HuffmanCode shorter = table.codes[first];
      const HuffmanCode longer = table.codes[second];
      const std::size_t extraBits = longer.length - shorter.length;
      if (first != second && shorter.length <= longer.length) {
        EXPECT_NE(static_cast<unsigned>(longer.bits) >> extraBits,
                  shorter.bits);
      }
    }
  }
}

/**
 * Given the table built from the shipped lengths
 * When the symbols of one length are taken in increasing order
 * Then their codes are consecutive numbers, as RFC 1951 section 3.2.2 says
 */
TEST(HuffmanTable, SymbolsOfOneLengthHaveConsecutiveCodes) {
  const HuffmanTable table = buildHuffmanTable(SERVER_TO_CLIENT_CODE_LENGTHS);

  for (std::size_t length = 1; length <= MAXIMUM_CODE_LENGTH; ++length) {
    bool seen = false;
    std::uint16_t previous = 0;
    for (const HuffmanCode code : table.codes) {
      if (code.length != length) {
        continue;
      }
      if (seen) {
        EXPECT_EQ(code.bits, previous + 1);
      }
      seen = true;
      previous = code.bits;
    }
  }
}
