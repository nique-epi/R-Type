#include "LeaderboardAnswer.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "LeaderboardRequest.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void requireValid(const LeaderboardRow& row) {
  if (row.rank == 0) {
    throw InvalidFieldValueException(RANK_FIELD, row.rank);
  }
  requireValidNickname(row.nickname);
}

void requireValid(const LeaderboardAnswer& message) {
  if (message.rows.size() > MAXIMUM_LEADERBOARD_ROWS) {
    throw TooManyLeaderboardRowsException(message.rows.size(),
                                          MAXIMUM_LEADERBOARD_ROWS);
  }
  for (const LeaderboardRow& row : message.rows) {
    requireValid(row);
  }
  if (message.ownRow.has_value()) {
    requireValid(*message.ownRow);
  }
}

void writeRow(ByteWriter& writer, const LeaderboardRow& row) {
  writer.writeUint32(row.rank);
  writer.writeUint32(row.score);
  writer.writeInt64(row.playedAt);
  writer.writeString(row.nickname);
}

LeaderboardRow readRow(ByteReader& reader) {
  return LeaderboardRow{.rank = reader.readUint32(),
                        .score = reader.readUint32(),
                        .playedAt = reader.readInt64(),
                        .nickname = reader.readString()};
}

/** @returns The count of rows, checked before anything is sized from it. */
std::uint8_t readRowCount(ByteReader& reader) {
  const std::uint8_t rowCount = reader.readUint8();
  if (rowCount > MAXIMUM_LEADERBOARD_ROWS) {
    throw TooManyLeaderboardRowsException(rowCount, MAXIMUM_LEADERBOARD_ROWS);
  }
  return rowCount;
}

}  // namespace

void encodePayload(ByteWriter& writer, const LeaderboardAnswer& message) {
  requireValid(message);
  encodePayload(writer, message.asked);
  writer.writeUint8(static_cast<std::uint8_t>(message.rows.size()));
  for (const LeaderboardRow& row : message.rows) {
    writeRow(writer, row);
  }
  writeBool(writer, message.ownRow.has_value());
  if (message.ownRow.has_value()) {
    writeRow(writer, *message.ownRow);
  }
}

LeaderboardAnswer decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<LeaderboardAnswer> type) {
  ByteReader reader{payload};
  LeaderboardAnswer message;
  message.asked = readLeaderboardRequest(reader);
  const std::uint8_t rowCount = readRowCount(reader);
  message.rows.reserve(rowCount);
  for (std::uint8_t index = 0; index < rowCount; ++index) {
    message.rows.push_back(readRow(reader));
  }
  if (readBool(reader)) {
    message.ownRow = readRow(reader);
  }
  requireFullyRead(reader);
  requireValid(message);
  return message;
}

}  // namespace rtype::network
