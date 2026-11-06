#include "ConnectionRequest.hpp"
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

/** @returns Whether @p choice names a game by a number, as ExistingGame does.
 * @throws InvalidFieldValueException when its kind is not one of
 * GameChoiceKind. */
bool namesAGame(const GameChoice& choice) {
  switch (choice.kind) {
    case GameChoiceKind::ExistingGame:
      return true;
    case GameChoiceKind::AnyGame:
    case GameChoiceKind::NewGame:
      return false;
  }
  throw InvalidFieldValueException(KIND_FIELD,
                                   static_cast<std::uint8_t>(choice.kind));
}

void validate(const GameChoice& choice) {
  const bool numbered = choice.number >= FIRST_HOSTED_GAME_NUMBER;
  if (namesAGame(choice) != numbered) {
    throw InvalidFieldValueException(NUMBER_FIELD, choice.number);
  }
}

void validate(const ConnectionRequest& message) {
  requireValidNickname(message.nickname);
  validate(message.game);
}

}  // namespace

void encodePayload(ByteWriter& writer, const ConnectionRequest& message) {
  validate(message);
  writer.writeUint64(message.cookie);
  writer.writeString(message.nickname);
  writer.writeUint8(static_cast<std::uint8_t>(message.game.kind));
  writer.writeUint16(message.game.number);
}

ConnectionRequest decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionRequest> type) {
  ByteReader reader{payload};
  ConnectionRequest message{
      .cookie = reader.readUint64(),
      .nickname = reader.readString(),
      .game =
          GameChoice{.kind = static_cast<GameChoiceKind>(reader.readUint8()),
                     .number = reader.readUint16()}};
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
