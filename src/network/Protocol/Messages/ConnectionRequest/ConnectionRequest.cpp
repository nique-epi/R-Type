#include "ConnectionRequest.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "GameRules.hpp"
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

bool opensAGame(const GameChoice& choice) {
  return choice.kind == GameChoiceKind::NewGame;
}

/** @throws InvalidFieldValueException when rules come with any choice but
 * NewGame, or none with it.
 * @throws InvalidDifficultyException when their difficulty breaks the rules of
 * a nickname. */
void validate(const std::optional<GameRules>& rules, const GameChoice& choice) {
  if (opensAGame(choice) != rules.has_value()) {
    throw InvalidFieldValueException(
        RULES_FIELD, static_cast<std::uint8_t>(opensAGame(choice)));
  }
  if (rules.has_value()) {
    requireValidDifficulty(rules->difficulty);
  }
}

void validate(const ConnectionRequest& message) {
  requireValidNickname(message.nickname);
  validate(message.game);
  validate(message.rules, message.game);
}

void encodeRules(ByteWriter& writer, const GameRules& rules) {
  writer.writeString(rules.difficulty);
  writer.writeUint8(rules.lives);
  writeBool(writer, rules.friendlyFire);
}

GameRules decodeRules(ByteReader& reader) {
  GameRules rules{.difficulty = reader.readString()};
  rules.lives = reader.readUint8();
  rules.friendlyFire = readBool(reader);
  return rules;
}

/** @returns The rules that follow the choice of a NewGame; nothing for any
 * other choice, which carries none. */
std::optional<GameRules> decodeRulesOf(ByteReader& reader,
                                       const GameChoice& choice) {
  if (!opensAGame(choice)) {
    return std::nullopt;
  }
  return decodeRules(reader);
}

}  // namespace

std::optional<GameRules> defaultRulesFor(const GameChoice& game) {
  if (!opensAGame(game)) {
    return std::nullopt;
  }
  return defaultGameRules();
}

void encodePayload(ByteWriter& writer, const ConnectionRequest& message) {
  validate(message);
  writer.writeUint64(message.cookie);
  writer.writeString(message.nickname);
  writer.writeUint8(static_cast<std::uint8_t>(message.game.kind));
  writer.writeUint16(message.game.number);
  if (message.rules.has_value()) {
    encodeRules(writer, *message.rules);
  }
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
  validate(message.game);
  message.rules = decodeRulesOf(reader, message.game);
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
