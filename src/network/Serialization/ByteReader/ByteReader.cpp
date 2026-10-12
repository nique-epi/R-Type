#include "ByteReader.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include "NetworkException.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {

ByteReader::ByteReader(std::span<const std::byte> buffer) : buffer_(buffer) {}

std::uint8_t ByteReader::readUint8() {
  return static_cast<std::uint8_t>(readUnsigned(UINT8_WIRE_SIZE));
}

std::uint16_t ByteReader::readUint16() {
  return static_cast<std::uint16_t>(readUnsigned(UINT16_WIRE_SIZE));
}

std::uint32_t ByteReader::readUint32() {
  return static_cast<std::uint32_t>(readUnsigned(UINT32_WIRE_SIZE));
}

std::uint64_t ByteReader::readUint64() {
  return readUnsigned(UINT64_WIRE_SIZE);
}

std::int8_t ByteReader::readInt8() {
  return static_cast<std::int8_t>(readUint8());
}

std::int16_t ByteReader::readInt16() {
  return static_cast<std::int16_t>(readUint16());
}

std::int32_t ByteReader::readInt32() {
  return static_cast<std::int32_t>(readUint32());
}

std::int64_t ByteReader::readInt64() {
  return static_cast<std::int64_t>(readUint64());
}

float ByteReader::readFloat() { return std::bit_cast<float>(readUint32()); }

std::string ByteReader::readString() {
  requireAvailable(SHORT_STRING_PREFIX_SIZE);
  const auto length = std::to_integer<std::size_t>(buffer_[position_]);
  requireAvailable(SHORT_STRING_PREFIX_SIZE + length);
  position_ += SHORT_STRING_PREFIX_SIZE;
  std::string value;
  value.reserve(length);
  for (std::size_t index = 0; index < length; ++index) {
    value.push_back(static_cast<char>(buffer_[position_ + index]));
  }
  position_ += length;
  return value;
}

std::size_t ByteReader::remaining() const { return buffer_.size() - position_; }

void ByteReader::requireAvailable(std::size_t byteCount) const {
  if (byteCount > remaining()) {
    throw BufferUnderflowException(byteCount, remaining());
  }
}

std::uint64_t ByteReader::readUnsigned(std::size_t byteCount) {
  requireAvailable(byteCount);
  std::uint64_t value = 0;
  for (std::size_t index = 0; index < byteCount; ++index) {
    value = (value << BITS_PER_BYTE) |
            std::to_integer<std::uint64_t>(buffer_[position_ + index]);
  }
  position_ += byteCount;
  return value;
}

}  // namespace rtype::network
