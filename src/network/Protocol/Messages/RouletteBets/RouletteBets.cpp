#include "RouletteBets.hpp"
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

/**
 * @throws InvalidFieldValueException when @p kind is not one of
 * RouletteBetKind.
 */
void requireListedKind(RouletteBetKind kind) {
  switch (kind) {
    case RouletteBetKind::Straight:
    case RouletteBetKind::Split:
    case RouletteBetKind::SplitAcross:
    case RouletteBetKind::Street:
    case RouletteBetKind::Corner:
    case RouletteBetKind::SixLine:
    case RouletteBetKind::Dozen:
    case RouletteBetKind::Column:
    case RouletteBetKind::Red:
    case RouletteBetKind::Black:
    case RouletteBetKind::Even:
    case RouletteBetKind::Odd:
    case RouletteBetKind::Low:
    case RouletteBetKind::High:
      return;
  }
  throw InvalidFieldValueException(KIND_FIELD, static_cast<std::uint8_t>(kind));
}

/**
 * @throws TooManyRouletteBetsException when @p count is above
 * MAXIMUM_ROULETTE_BETS.
 */
void requireBetCount(std::size_t count) {
  if (count > MAXIMUM_ROULETTE_BETS) {
    throw TooManyRouletteBetsException(count, MAXIMUM_ROULETTE_BETS);
  }
}

/**
 * @throws InvalidFieldValueException when the kind, the first number or the
 * amount of @p bet is not one the protocol allows.
 */
void requireValidBet(const RouletteBet& bet) {
  requireListedKind(bet.kind);
  if (bet.firstNumber > HIGHEST_ROULETTE_NUMBER) {
    throw InvalidFieldValueException(FIRST_NUMBER_FIELD, bet.firstNumber);
  }
  if (bet.amount == 0) {
    throw InvalidFieldValueException(AMOUNT_FIELD, bet.amount);
  }
}

void validate(const RouletteBets& message) {
  requireBetCount(message.bets.size());
  for (const RouletteBet& bet : message.bets) {
    requireValidBet(bet);
  }
}

RouletteBet readBet(ByteReader& reader) {
  const auto kind = static_cast<RouletteBetKind>(reader.readUint8());
  const std::uint8_t firstNumber = reader.readUint8();
  return RouletteBet{
      .kind = kind, .firstNumber = firstNumber, .amount = reader.readUint32()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const RouletteBets& message) {
  validate(message);
  writer.writeUint16(message.round);
  writeBool(writer, message.spinVote);
  writer.writeUint8(static_cast<std::uint8_t>(message.bets.size()));
  for (const RouletteBet& bet : message.bets) {
    writer.writeUint8(static_cast<std::uint8_t>(bet.kind));
    writer.writeUint8(bet.firstNumber);
    writer.writeUint32(bet.amount);
  }
}

RouletteBets decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<RouletteBets> type) {
  ByteReader reader{payload};
  RouletteBets message{
      .round = reader.readUint16(), .spinVote = readBool(reader), .bets = {}};
  const std::uint8_t betCount = reader.readUint8();
  requireBetCount(betCount);
  message.bets.reserve(betCount);
  for (std::uint8_t index = 0; index < betCount; ++index) {
    message.bets.push_back(readBet(reader));
  }
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
