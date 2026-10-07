#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace rtype::network {

/**
 * @brief Appends fixed-size values to a byte buffer in network byte order.
 *
 * Integers are written big-endian whatever the host is. Floats are IEEE-754
 * binary32. Strings are a one-byte length followed by the raw bytes, so they
 * are limited to MAX_SHORT_STRING_LENGTH bytes; a longer one raises
 * StringTooLongException and leaves the buffer untouched.
 */
class ByteWriter {
 public:
  void writeUint8(std::uint8_t value);
  void writeUint16(std::uint16_t value);
  void writeUint32(std::uint32_t value);
  void writeUint64(std::uint64_t value);
  void writeInt8(std::int8_t value);
  void writeInt16(std::int16_t value);
  void writeInt32(std::int32_t value);
  void writeInt64(std::int64_t value);
  void writeFloat(float value);
  void writeString(std::string_view value);

  [[nodiscard]] const std::vector<std::byte>& bytes() const;

 private:
  std::vector<std::byte> bytes_;
};

}  // namespace rtype::network
