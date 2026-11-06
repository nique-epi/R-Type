#include "GameStatus.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "GameStatusMeasures.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

bool isListed(GameStatusState state) {
  switch (state) {
    case GameStatusState::WaitingForPlayers:
    case GameStatusState::InProgress:
    case GameStatusState::Finished:
      return true;
  }
  return false;
}

void requireListed(GameStatusState state) {
  if (!isListed(state)) {
    throw InvalidFieldValueException(STATE_FIELD,
                                     static_cast<std::uint8_t>(state));
  }
}

void validate(const GameStatus& message) {
  requireListed(message.state);
  if (message.nicknames.size() > MAXIMUM_PLAYERS) {
    throw TooManyPlayersException(message.nicknames.size(), MAXIMUM_PLAYERS);
  }
  for (const std::string& nickname : message.nicknames) {
    requireValidNickname(nickname);
  }
}

/** @returns The measures of @p measures, in the order of their bits. */
std::array<std::optional<std::uint32_t>, GAME_STATUS_MEASURE_COUNT> inBitOrder(
    const GameStatusMeasures& measures) {
  return {measures.tickMedianMicroseconds,
          measures.tickNinetyFifthPercentileMicroseconds,
          measures.averageRoundTripMicroseconds};
}

/** @returns The measures byte of @p measures: one bit per measure taken. */
std::uint8_t presenceOf(const GameStatusMeasures& measures) {
  std::uint8_t presence = 0;
  std::size_t position = 0;
  for (const std::optional<std::uint32_t>& measure : inBitOrder(measures)) {
    if (measure.has_value()) {
      presence = static_cast<std::uint8_t>(presence | (1U << position));
    }
    ++position;
  }
  return presence;
}

void writeMeasures(ByteWriter& writer, const GameStatusMeasures& measures) {
  writer.writeUint8(presenceOf(measures));
  for (const std::optional<std::uint32_t>& measure : inBitOrder(measures)) {
    if (measure.has_value()) {
      writer.writeUint32(*measure);
    }
  }
}

/** @returns The measure of bit @p position, read when @p presence sets it. */
std::optional<std::uint32_t> readMeasure(ByteReader& reader,
                                         std::uint8_t presence,
                                         std::uint8_t position) {
  if ((presence & (1U << position)) == 0) {
    return std::nullopt;
  }
  return reader.readUint32();
}

/**
 * @throws InvalidFieldValueException when the measures byte sets a bit no
 * measure uses, before any measure is read.
 */
GameStatusMeasures readMeasures(ByteReader& reader) {
  const std::uint8_t presence = reader.readUint8();
  if ((presence & ~static_cast<unsigned int>(GAME_STATUS_MEASURE_BITS)) != 0U) {
    throw InvalidFieldValueException(MEASURES_FIELD, presence);
  }
  GameStatusMeasures measures;
  measures.tickMedianMicroseconds =
      readMeasure(reader, presence, TICK_MEDIAN_MEASURE_BIT_POSITION);
  measures.tickNinetyFifthPercentileMicroseconds = readMeasure(
      reader, presence, TICK_NINETY_FIFTH_PERCENTILE_MEASURE_BIT_POSITION);
  measures.averageRoundTripMicroseconds =
      readMeasure(reader, presence, AVERAGE_ROUND_TRIP_MEASURE_BIT_POSITION);
  return measures;
}

}  // namespace

void encodePayload(ByteWriter& writer, const GameStatus& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.state));
  writer.writeUint8(static_cast<std::uint8_t>(message.nicknames.size()));
  for (const std::string& nickname : message.nicknames) {
    writer.writeString(nickname);
  }
  writeMeasures(writer, message.measures);
}

GameStatus decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<GameStatus> type) {
  ByteReader reader{payload};
  GameStatus message;
  message.state = static_cast<GameStatusState>(reader.readUint8());
  requireListed(message.state);
  const std::uint8_t nicknameCount = readPlayerCount(reader);
  message.nicknames.reserve(nicknameCount);
  for (std::uint8_t index = 0; index < nicknameCount; ++index) {
    message.nicknames.push_back(reader.readString());
  }
  message.measures = readMeasures(reader);
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
