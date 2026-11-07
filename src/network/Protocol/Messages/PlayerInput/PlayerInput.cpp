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

void requireValidActions(std::uint8_t actions) {
  const auto unlistedBits = static_cast<std::uint8_t>(
      actions & static_cast<std::uint8_t>(~PLAYER_ACTION_BITS));
  if (unlistedBits != 0) {
    throw InvalidFieldValueException(ACTIONS_FIELD, actions);
  }
}

/**
 * Rejects a number of earlier actions that the sender cannot have written: more
 * than MAXIMUM_REPEATED_INPUTS, or reaching below FIRST_INPUT_NUMBER. Checked
 * before anything is allocated or read for them.
 */
void requireValidEarlierCount(std::uint32_t inputNumber, std::size_t count) {
  const bool reachesBelowTheFirst =
      count > 0 && count + FIRST_INPUT_NUMBER > inputNumber;
  if (count > MAXIMUM_REPEATED_INPUTS || reachesBelowTheFirst) {
    throw InvalidFieldValueException(EARLIER_COUNT_FIELD, count);
  }
}

void validate(const PlayerInput& message) {
  requireValidActions(message.actions);
  requireValidEarlierCount(message.inputNumber, message.earlierActions.size());
  for (const std::uint8_t actions : message.earlierActions) {
    requireValidActions(actions);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const PlayerInput& message) {
  validate(message);
  writer.writeUint32(message.inputNumber);
  writer.writeUint8(message.actions);
  writer.writeUint8(static_cast<std::uint8_t>(message.earlierActions.size()));
  for (const std::uint8_t actions : message.earlierActions) {
    writer.writeUint8(actions);
  }
  writer.writeUint32(message.shownTick);
}

PlayerInput decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<PlayerInput> type) {
  ByteReader reader{payload};
  PlayerInput message{.inputNumber = reader.readUint32(),
                      .actions = reader.readUint8(),
                      .earlierActions = {},
                      .shownTick = NO_SHOWN_TICK};
  const std::uint8_t earlierCount = reader.readUint8();
  requireValidEarlierCount(message.inputNumber, earlierCount);
  message.earlierActions.reserve(earlierCount);
  for (std::uint8_t index = 0; index < earlierCount; ++index) {
    message.earlierActions.push_back(reader.readUint8());
  }
  message.shownTick = reader.readUint32();
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
