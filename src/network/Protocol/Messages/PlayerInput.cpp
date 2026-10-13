#include "PlayerInput.hpp"
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

void validate(const PlayerInput& message) {
  const auto unlistedBits = static_cast<std::uint8_t>(
      message.actions & static_cast<std::uint8_t>(~PLAYER_ACTION_BITS));
  if (unlistedBits != 0) {
    throw InvalidFieldValueException(ACTIONS_FIELD, message.actions);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const PlayerInput& message) {
  validate(message);
  writer.writeUint32(message.inputNumber);
  writer.writeUint8(message.actions);
}

PlayerInput decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<PlayerInput> type) {
  ByteReader reader{payload};
  const PlayerInput message{.inputNumber = reader.readUint32(),
                            .actions = reader.readUint8()};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
