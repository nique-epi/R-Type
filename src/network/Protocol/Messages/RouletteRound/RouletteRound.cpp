#include "RouletteRound.hpp"
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
 * @throws InvalidFieldValueException when @p phase is not one of
 * RoulettePhase.
 */
void requireListedPhase(RoulettePhase phase) {
  switch (phase) {
    case RoulettePhase::Betting:
    case RoulettePhase::Spinning:
    case RoulettePhase::Result:
      return;
  }
  throw InvalidFieldValueException(PHASE_FIELD,
                                   static_cast<std::uint8_t>(phase));
}

void validate(const RouletteRound& message) {
  requireListedPhase(message.phase);
  if (message.winningNumber > HIGHEST_ROULETTE_NUMBER) {
    throw InvalidFieldValueException(WINNING_NUMBER_FIELD,
                                     message.winningNumber);
  }
  requireValidPlayers(std::span<const RouletteSeat>{message.seats});
}

RouletteSeat readSeat(ByteReader& reader) {
  const std::uint8_t playerNumber = reader.readUint8();
  const std::uint32_t stake = reader.readUint32();
  return RouletteSeat{.playerNumber = playerNumber,
                      .stake = stake,
                      .payout = reader.readUint32()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const RouletteRound& message) {
  validate(message);
  writer.writeUint16(message.round);
  writer.writeUint8(static_cast<std::uint8_t>(message.phase));
  writer.writeUint16(message.ticksLeft);
  writer.writeUint8(message.winningNumber);
  writer.writeUint8(static_cast<std::uint8_t>(message.seats.size()));
  for (const RouletteSeat& seat : message.seats) {
    writer.writeUint8(seat.playerNumber);
    writer.writeUint32(seat.stake);
    writer.writeUint32(seat.payout);
  }
}

RouletteRound decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<RouletteRound> type) {
  ByteReader reader{payload};
  RouletteRound message{.round = reader.readUint16(),
                        .phase = static_cast<RoulettePhase>(reader.readUint8()),
                        .ticksLeft = reader.readUint16(),
                        .winningNumber = reader.readUint8(),
                        .seats = {}};
  requireListedPhase(message.phase);
  const std::uint8_t seatCount = readPlayerCount(reader);
  message.seats.reserve(seatCount);
  for (std::uint8_t index = 0; index < seatCount; ++index) {
    message.seats.push_back(readSeat(reader));
  }
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network
