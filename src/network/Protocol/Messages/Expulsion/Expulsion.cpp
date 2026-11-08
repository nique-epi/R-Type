#include "Expulsion.hpp"
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

bool isListed(ExpulsionReason reason) {
  switch (reason) {
    case ExpulsionReason::ByTheHost:
    case ExpulsionReason::Banned:
      return true;
  }
  return false;
}

void validate(const Expulsion& message) {
  if (!isListed(message.reason)) {
    throw InvalidFieldValueException(REASON_FIELD,
                                     static_cast<std::uint8_t>(message.reason));
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const Expulsion& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.reason));
}

Expulsion decodePayload(std::span<const std::byte> payload,
                        [[maybe_unused]] std::type_identity<Expulsion> type) {
  ByteReader reader{payload};
  const Expulsion message{.reason =
                              static_cast<ExpulsionReason>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
