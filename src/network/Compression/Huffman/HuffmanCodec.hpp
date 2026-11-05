#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace rtype::network {

/**
 * @brief Codes a datagram body with the table the server sends with: each byte
 * becomes its Huffman code, then the end-of-body symbol, then zero bits to fill
 * the last byte. Nothing is kept between two calls.
 *
 * @returns The coded body. It can be longer than @p body, whose caller then
 * keeps the original.
 */
[[nodiscard]] std::vector<std::byte> huffmanEncode(
    std::span<const std::byte> body);

/**
 * @brief Reads back a body coded by huffmanEncode.
 *
 * It never produces more than @p maximumSize bytes, whatever @p coded holds,
 * and keeps nothing between two calls. A datagram a stranger sent can be
 * given to it.
 *
 * @returns The body, or std::nullopt when @p coded is not exactly one coded
 * body: it ends before the end symbol, holds more than @p maximumSize bytes,
 * has a set padding bit, or a whole byte after the end symbol.
 */
[[nodiscard]] std::optional<std::vector<std::byte>> huffmanDecode(
    std::span<const std::byte> coded, std::size_t maximumSize);

}  // namespace rtype::network
