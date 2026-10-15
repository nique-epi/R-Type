#include "ConnectionRefused.hpp"
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

bool isListed(RefusalReason reason) {
  switch (reason) {
    case RefusalReason::GameFull:
    case RefusalReason::GameInProgress:
    case RefusalReason::IncompatibleVersion:
      return true;
  }
  return false;
}

void validate(const ConnectionRefused& message) {
  if (!isListed(message.reason)) {
    throw InvalidFieldValueException(REASON_FIELD,
                                     static_cast<std::uint8_t>(message.reason));
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionRefused& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.reason));
}

ConnectionRefused decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionRefused> type) {
  ByteReader reader{payload};
  const ConnectionRefused message{
      .reason = static_cast<RefusalReason>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
