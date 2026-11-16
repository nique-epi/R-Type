#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "RoulettePhase.hpp"
#include "RouletteSeat.hpp"

namespace rtype::network {

/**
 * @brief The state of the roulette table of the waiting room: its round,
 * numbered from 0 and compared as a serial number, its phase, the ticks left
 * in that phase, the number drawn (meaningful from Spinning on) and the
 * players seated at the table. The server repeats it to every player of the
 * waiting room while the table is open.
 */
struct RouletteRound {
  static constexpr MessageType TYPE = MessageType::RouletteRound;

  std::uint16_t round{};
  RoulettePhase phase{RoulettePhase::Betting};
  std::uint16_t ticksLeft{};
  std::uint8_t winningNumber{};
  std::vector<RouletteSeat> seats;

  bool operator==(const RouletteRound&) const = default;
};

/**
 * @brief Writes the round, the phase, the ticks left, the winning number, the
 * number of seats, then each one: player number, stake, payout.
 * @throws InvalidFieldValueException when the phase is not one of
 * RoulettePhase or the winning number is above HIGHEST_ROULETTE_NUMBER.
 * @throws TooManyPlayersException, InvalidFieldValueException or
 * DuplicatePlayerNumberException for seats the protocol does not allow.
 * Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const RouletteRound& message);

/**
 * @brief Reads a RouletteRound from the whole of @p payload. The number of
 * seats is checked before any seat is read.
 * @throws TooManyPlayersException when it announces more than MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException or DuplicatePlayerNumberException for a
 * phase, a number or a seat the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last seat.
 */
[[nodiscard]] RouletteRound decodePayload(
    std::span<const std::byte> payload, std::type_identity<RouletteRound> type);

}  // namespace rtype::network
