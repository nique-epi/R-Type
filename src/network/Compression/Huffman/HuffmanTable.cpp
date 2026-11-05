#include "HuffmanTable.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include "CompressionConstants.hpp"
#include "HuffmanCode.hpp"
#include "HuffmanTableConstants.hpp"

namespace rtype::network {

namespace {

using CodeLengths = std::array<std::uint8_t, HUFFMAN_SYMBOL_COUNT>;
using ValueByLength = std::array<std::uint16_t, MAXIMUM_CODE_LENGTH + 1>;

ValueByLength countCodesByLength(const CodeLengths& lengths) {
  ValueByLength counts{};
  for (const std::uint8_t length : lengths) {
    ++counts[length];
  }
  counts[0] = 0;
  return counts;
}

/** @returns The first code of each length, as step 2 of RFC 1951 3.2.2. */
ValueByLength firstCodeOfEachLength(const ValueByLength& counts) {
  ValueByLength firstCodes{};
  std::uint16_t code = 0;
  for (std::size_t length = 1; length <= MAXIMUM_CODE_LENGTH; ++length) {
    code = static_cast<std::uint16_t>(
        (static_cast<unsigned>(code) + counts[length - 1]) << 1U);
    firstCodes[length] = code;
  }
  return firstCodes;
}

/** @returns The rank, in symbolsByCode, of the first symbol of each length. */
ValueByLength firstRankOfEachLength(const ValueByLength& counts) {
  ValueByLength firstRanks{};
  for (std::size_t length = 1; length < MAXIMUM_CODE_LENGTH; ++length) {
    firstRanks[length + 1] =
        static_cast<std::uint16_t>(firstRanks[length] + counts[length]);
  }
  return firstRanks;
}

}  // namespace

HuffmanTable buildHuffmanTable(const CodeLengths& lengths) {
  HuffmanTable table;
  table.countByLength = countCodesByLength(lengths);
  ValueByLength nextCodes = firstCodeOfEachLength(table.countByLength);
  ValueByLength nextRanks = firstRankOfEachLength(table.countByLength);
  for (std::size_t symbol = 0; symbol < HUFFMAN_SYMBOL_COUNT; ++symbol) {
    const std::uint8_t length = lengths[symbol];
    table.codes[symbol] =
        HuffmanCode{.bits = nextCodes[length]++, .length = length};
    table.symbolsByCode[nextRanks[length]++] =
        static_cast<std::uint16_t>(symbol);
  }
  return table;
}

const HuffmanTable& serverToClientTable() {
  static const HuffmanTable table =
      buildHuffmanTable(SERVER_TO_CLIENT_CODE_LENGTHS);
  return table;
}

}  // namespace rtype::network
