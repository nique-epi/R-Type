#include "WireTypes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string_view>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "Utf8Constants.hpp"

namespace rtype::network {

namespace {

bool contains(ByteRange range, std::uint8_t byte) {
  return byte >= range.minimum && byte <= range.maximum;
}

std::uint8_t toByte(char character) {
  return static_cast<std::uint8_t>(character);
}

std::optional<Utf8SequenceRule> ruleStartingWith(std::uint8_t lead) {
  for (const Utf8SequenceRule& rule : UTF8_SEQUENCE_RULES) {
    if (contains(rule.lead, lead)) {
      return rule;
    }
  }
  return std::nullopt;
}

/**
 * @brief Checks the bytes of one sequence after its lead byte; @p sequence
 * holds exactly the length of @p rule.
 */
bool hasValidContinuation(std::string_view sequence,
                          const Utf8SequenceRule& rule) {
  const std::string_view continuation = sequence.substr(1);
  if (continuation.empty()) {
    return true;
  }
  if (!contains(rule.second, toByte(continuation.front()))) {
    return false;
  }
  return std::ranges::all_of(continuation.substr(1), [](char character) {
    return contains(CONTINUATION_BYTES, toByte(character));
  });
}

}  // namespace

void writeBool(ByteWriter& writer, bool value) {
  writer.writeUint8(value ? TRUE_WIRE_VALUE : FALSE_WIRE_VALUE);
}

bool readBool(ByteReader& reader) {
  const std::uint8_t value = reader.readUint8();
  if (value != FALSE_WIRE_VALUE && value != TRUE_WIRE_VALUE) {
    throw InvalidBoolException(value);
  }
  return value == TRUE_WIRE_VALUE;
}

void writeCoordinate(ByteWriter& writer, float coordinate) {
  if (std::isnan(coordinate)) {
    throw InvalidCoordinateException();
  }
  const float steps = std::round(coordinate * WIRE_COORDINATE_STEPS_PER_UNIT);
  writer.writeInt16(static_cast<std::int16_t>(
      std::clamp(steps, MINIMUM_WIRE_STEPS, MAXIMUM_WIRE_STEPS)));
}

float readCoordinate(ByteReader& reader) {
  return static_cast<float>(reader.readInt16()) /
         WIRE_COORDINATE_STEPS_PER_UNIT;
}

bool isValidUtf8(std::string_view text) {
  std::string_view rest = text;
  while (!rest.empty()) {
    const std::optional<Utf8SequenceRule> rule =
        ruleStartingWith(toByte(rest.front()));
    if (!rule || rest.size() < rule->length ||
        !hasValidContinuation(rest.substr(0, rule->length), *rule)) {
      return false;
    }
    rest.remove_prefix(rule->length);
  }
  return true;
}

bool isValidNickname(std::string_view nickname) {
  return nickname.size() >= MINIMUM_NICKNAME_SIZE &&
         nickname.size() <= MAXIMUM_NICKNAME_SIZE && isValidUtf8(nickname);
}

void requireValidNickname(std::string_view nickname) {
  if (!isValidNickname(nickname)) {
    throw InvalidNicknameException(nickname.size());
  }
}

void requireValidPlayerNumber(std::uint8_t playerNumber) {
  if (playerNumber < FIRST_PLAYER_NUMBER || playerNumber > MAXIMUM_PLAYERS) {
    throw InvalidFieldValueException(PLAYER_NUMBER_FIELD, playerNumber);
  }
}

void requireFullyRead(const ByteReader& reader) {
  if (reader.remaining() != 0) {
    throw TrailingBytesException(reader.remaining());
  }
}

std::uint8_t readPlayerCount(ByteReader& reader) {
  const std::uint8_t playerCount = reader.readUint8();
  if (playerCount > MAXIMUM_PLAYERS) {
    throw TooManyPlayersException(playerCount, MAXIMUM_PLAYERS);
  }
  return playerCount;
}

}  // namespace rtype::network
