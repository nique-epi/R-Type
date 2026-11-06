#include "ConnectionRedirect.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const ConnectionRedirect& message) {
  if (message.port < LOWEST_PORT) {
    throw InvalidFieldValueException(PORT_FIELD, message.port);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionRedirect& message) {
  validate(message);
  writer.writeUint16(message.port);
}

ConnectionRedirect decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionRedirect> type) {
  ByteReader reader{payload};
  const ConnectionRedirect message{.port = reader.readUint16()};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
