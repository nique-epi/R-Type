#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace rtype::network {

/**
 * @brief Reads fixed-size values from a byte buffer in network byte order.
 *
 * The reader does not own the buffer: the caller keeps it alive for as long
 * as the reader is used. A read that needs more bytes than remain raises
 * BufferUnderflowException and leaves the position unchanged.
 */
class ByteReader {
 public:
  explicit ByteReader(std::span<const std::byte> buffer);

  std::uint8_t readUint8();
  std::uint16_t readUint16();
  std::uint32_t readUint32();
  std::uint64_t readUint64();
  std::int8_t readInt8();
  std::int16_t readInt16();
  std::int32_t readInt32();
  std::int64_t readInt64();
  float readFloat();
  std::string readString();

  [[nodiscard]] std::size_t remaining() const;

 private:
  void requireAvailable(std::size_t byteCount) const;
  std::uint64_t readUnsigned(std::size_t byteCount);

  std::span<const std::byte> buffer_;
  std::size_t position_ = 0;
};

}  // namespace rtype::network
