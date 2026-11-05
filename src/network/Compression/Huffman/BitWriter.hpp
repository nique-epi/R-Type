#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "HuffmanCode.hpp"

namespace rtype::network {

/**
 * @brief Writes codes into bytes, most significant bit first.
 */
class BitWriter {
 public:
  /** @brief Appends the bits of @p code. */
  void write(HuffmanCode code);

  /** @returns The bytes written, the last one filled with zero bits. */
  [[nodiscard]] std::vector<std::byte> finish();

 private:
  void appendBit(unsigned bit);

  std::vector<std::byte> bytes_;
  std::uint8_t pending_{0};
  std::size_t pendingBitCount_{0};
};

}  // namespace rtype::network
