#include "LeaderboardRequest.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void requireValid(const LeaderboardRequest& message) {
  if (message.playerCount != ALL_PLAYER_COUNTS &&
      (message.playerCount < FIRST_PLAYER_NUMBER ||
       message.playerCount > MAXIMUM_PLAYERS)) {
    throw InvalidFieldValueException(PLAYER_COUNT_FIELD, message.playerCount);
  }
  if (message.difficulty != ALL_DIFFICULTIES) {
    requireValidDifficulty(message.difficulty);
  }
}

}  // namespace

std::optional<std::string> difficultyFilterOf(
    const LeaderboardRequest& request) {
  if (request.difficulty == ALL_DIFFICULTIES) {
    return std::nullopt;
  }
  return request.difficulty;
}

std::optional<std::uint8_t> playerCountFilterOf(
    const LeaderboardRequest& request) {
  if (request.playerCount == ALL_PLAYER_COUNTS) {
    return std::nullopt;
  }
  return request.playerCount;
}

void encodePayload(ByteWriter& writer, const LeaderboardRequest& message) {
  requireValid(message);
  writer.writeUint8(message.playerCount);
  writer.writeString(message.difficulty);
}

LeaderboardRequest readLeaderboardRequest(ByteReader& reader) {
  LeaderboardRequest message;
  message.playerCount = reader.readUint8();
  message.difficulty = reader.readString();
  requireValid(message);
  return message;
}

LeaderboardRequest decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<LeaderboardRequest> type) {
  ByteReader reader{payload};
  LeaderboardRequest message = readLeaderboardRequest(reader);
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network
