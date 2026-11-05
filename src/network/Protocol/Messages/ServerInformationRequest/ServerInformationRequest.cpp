#include "ServerInformationRequest.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/**
 * @brief Checks that every byte of @p padding is PADDING_BYTE.
 * @throws InvalidFieldValueException naming PADDING_FIELD with the first byte
 * that is not.
 */
void requireZeroPadding(std::span<const std::byte> padding) {
  for (const std::byte value : padding) {
    if (value != std::byte{PADDING_BYTE}) {
      throw InvalidFieldValueException(PADDING_FIELD,
                                       std::to_integer<std::uint8_t>(value));
    }
  }
}

}  // namespace

void encodePayload(ByteWriter& writer,
                   const ServerInformationRequest& message) {
  static constexpr std::array<std::byte,
                              SERVER_INFORMATION_REQUEST_PADDING_SIZE>
      padding{};
  static_assert(PADDING_BYTE == 0, "an array of zeros is the padding");
  writer.writeUint32(message.sentAt);
  writer.writeBytes(padding);
}

ServerInformationRequest decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ServerInformationRequest> type) {
  ByteReader reader{payload};
  const ServerInformationRequest message{.sentAt = reader.readUint32()};
  requireZeroPadding(reader.readBytes(SERVER_INFORMATION_REQUEST_PADDING_SIZE));
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network
