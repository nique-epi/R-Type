#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace rtype::network {

/**
 * @brief Reads bytes one bit at a time, most significant bit first.
 *
 * It keeps a view of the bytes: they must outlive the reader.
 */
class BitReader {
 public:
  explicit BitReader(std::span<const std::byte> bytes);

  /** @returns The next bit, or std::nullopt when every bit was read. */
  [[nodiscard]] std::optional<std::uint8_t> next();

  /**
   * @returns Whether what is left is padding: fewer than a whole byte of bits,
   * every one of them 0. A coded body ends in no other way, so two different
   * byte strings never decode to the same body.
   */
  [[nodiscard]] bool onlyZeroPaddingLeft() const;

 private:
  [[nodiscard]] std::uint8_t bitAt(std::size_t position) const;

  std::span<const std::byte> bytes_;
  std::size_t position_{0};
};

}  // namespace rtype::network
