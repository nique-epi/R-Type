#include "PlayerLeft.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

bool isListed(DepartureReason reason) {
  switch (reason) {
    case DepartureReason::Left:
    case DepartureReason::TimedOut:
      return true;
  }
  return false;
}

void validate(const PlayerLeft& message) {
  requireValidPlayerNumber(message.playerNumber);
  if (!isListed(message.reason)) {
    throw InvalidFieldValueException("reason",
                                     static_cast<std::uint8_t>(message.reason));
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const PlayerLeft& message) {
  validate(message);
  writer.writeUint8(message.playerNumber);
  writer.writeUint8(static_cast<std::uint8_t>(message.reason));
}

PlayerLeft decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<PlayerLeft> type) {
  ByteReader reader{payload};
  const PlayerLeft message{
      .playerNumber = reader.readUint8(),
      .reason = static_cast<DepartureReason>(reader.readUint8())};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
