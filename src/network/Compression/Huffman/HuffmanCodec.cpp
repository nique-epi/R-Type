#include "HuffmanCodec.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "CompressionConstants.hpp"
#include "HuffmanTable.hpp"

namespace rtype::network {

namespace {

/**
 * @brief Reads one symbol as puff.c does: adds a bit to the code and checks
 * whether it falls among the codes of that length.
 *
 * @returns The symbol, or std::nullopt when the bits run out or no code of
 * MAXIMUM_CODE_LENGTH bits matches.
 */
std::optional<std::size_t> readSymbol(BitReader& reader,
                                      const HuffmanTable& table) {
  std::uint32_t code = 0;
  std::uint32_t firstCode = 0;
  std::uint32_t firstRank = 0;
  for (std::size_t length = 1; length <= MAXIMUM_CODE_LENGTH; ++length) {
    const std::optional<std::uint8_t> bit = reader.next();
    if (!bit.has_value()) {
      return std::nullopt;
    }
    code |= *bit;
    const std::uint32_t count = table.countByLength[length];
    if (code < firstCode + count) {
      return table.symbolsByCode[firstRank + (code - firstCode)];
    }
    firstRank += count;
    firstCode = (firstCode + count) << 1U;
    code <<= 1U;
  }
  return std::nullopt;
}

}  // namespace

std::vector<std::byte> huffmanEncode(std::span<const std::byte> body) {
  const HuffmanTable& table = serverToClientTable();
  BitWriter writer;
  for (const std::byte byte : body) {
    writer.write(table.codes[std::to_integer<std::size_t>(byte)]);
  }
  writer.write(table.codes[END_OF_BODY_SYMBOL]);
  return writer.finish();
}

std::optional<std::vector<std::byte>> huffmanDecode(
    std::span<const std::byte> coded, std::size_t maximumSize) {
  const HuffmanTable& table = serverToClientTable();
  BitReader reader{coded};
  std::vector<std::byte> body;
  std::optional<std::size_t> symbol = readSymbol(reader, table);
  while (symbol != END_OF_BODY_SYMBOL) {
    if (!symbol.has_value() || body.size() == maximumSize) {
      return std::nullopt;
    }
    body.push_back(static_cast<std::byte>(*symbol));
    symbol = readSymbol(reader, table);
  }
  if (!reader.onlyZeroPaddingLeft()) {
    return std::nullopt;
  }
  return body;
}

}  // namespace rtype::network
