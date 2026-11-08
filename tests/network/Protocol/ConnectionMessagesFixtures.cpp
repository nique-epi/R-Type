#include "ConnectionMessagesFixtures.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include "ByteWriter.hpp"
#include "ConnectionRequest.hpp"
#include "GameRules.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

ConnectionRequest smallestRequest() {
  return ConnectionRequest{.cookie = 0,
                           .nickname = std::string{ONE_BYTE_NICKNAME}};
}

ConnectionRequest requestFor(const GameChoice& game) {
  return ConnectionRequest{.cookie = SOME_COOKIE,
                           .nickname = std::string{ONE_BYTE_NICKNAME},
                           .game = game,
                           .rules = defaultRulesFor(game)};
}

ConnectionRequest newGameRequestWith(GameRules rules) {
  ConnectionRequest request = requestFor(NEW_GAME_CHOICE);
  request.rules = std::move(rules);
  return request;
}

GameRules hardRulesWithFriendlyFire() {
  return GameRules{.difficulty = std::string{HARD_DIFFICULTY},
                   .lives = FIVE_LIVES,
                   .friendlyFire = true};
}

void writeAnyGameChoice(ByteWriter& writer) {
  writer.writeUint8(ANY_GAME_KIND_BYTE);
  writer.writeUint16(NO_NAMED_GAME);
}

void writeRequestFor(ByteWriter& writer, const GameChoice& game) {
  writer.writeUint64(SOME_COOKIE);
  writer.writeString(ONE_BYTE_NICKNAME);
  writer.writeUint8(static_cast<std::uint8_t>(game.kind));
  writer.writeUint16(game.number);
}

}  // namespace rtype::network::testing
