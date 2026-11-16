#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "RouletteBet.hpp"

namespace rtype::network {

/**
 * @brief Every bet the player holds on the cloth for one round, and whether
 * the player asks to spin now. The client sends it on each change and again
 * every 500 ms while the bets are open: an absolute state, never a change.
 */
struct RouletteBets {
  static constexpr MessageType TYPE = MessageType::RouletteBets;

  std::uint16_t round{};
  bool spinVote{};
  std::vector<RouletteBet> bets;

  bool operator==(const RouletteBets&) const = default;
};

/**
 * @brief Writes the round, the vote, the number of bets, then each bet: kind,
 * first number, amount.
 * @throws TooManyRouletteBetsException when there are more than
 * MAXIMUM_ROULETTE_BETS.
 * @throws InvalidFieldValueException when a kind is not one of
 * RouletteBetKind, a first number is above HIGHEST_ROULETTE_NUMBER or an
 * amount is 0. Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const RouletteBets& message);

/**
 * @brief Reads a RouletteBets from the whole of @p payload. The number of
 * bets is checked before any bet is read.
 * @throws TooManyRouletteBetsException when it announces more than
 * MAXIMUM_ROULETTE_BETS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidBoolException or InvalidFieldValueException for a vote or a
 * bet the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the last bet.
 */
[[nodiscard]] RouletteBets decodePayload(std::span<const std::byte> payload,
                                         std::type_identity<RouletteBets> type);

}  // namespace rtype::network
