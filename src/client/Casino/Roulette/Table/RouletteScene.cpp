#include "RouletteScene.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ClothPlace.hpp"
#include "RouletteBand.hpp"
#include "RouletteBarStates.hpp"
#include "RouletteBet.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteTexts.hpp"
#include "RouletteView.hpp"
#include "WheelMoment.hpp"
#include "WheelMotion.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

namespace {

using network::RoulettePhase;

/** @returns Whether the room of @p reading takes bets. */
bool betsOpen(const RouletteReading& reading) {
  return reading.room.has_value() &&
         reading.room->phase == RoulettePhase::Betting;
}

/** @returns The chips of @p bets added up. */
std::uint64_t stakeOf(const std::vector<network::RouletteBet>& bets) {
  std::uint64_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return stake;
}

/** @returns The chips the band shows for @p reading. */
std::uint32_t chipsShownOf(const RouletteReading& reading) {
  if (!reading.room.has_value() ||
      reading.room->phase == RoulettePhase::Result) {
    return reading.walletChips;
  }
  if (reading.room->phase == RoulettePhase::Spinning) {
    return reading.chipsAtHand > reading.acceptedStake
               ? reading.chipsAtHand - reading.acceptedStake
               : 0;
  }
  const std::uint64_t stake = stakeOf(reading.bets);
  return reading.chipsAtHand > stake
             ? static_cast<std::uint32_t>(reading.chipsAtHand - stake)
             : 0;
}

/** @returns What the wheel shows for @p reading: until the number is known,
 * the rotor turns towards the zero, and the wheel waits at rest at a result
 * without it. */
WheelPicture wheelOf(const RouletteReading& reading) {
  if (!reading.room.has_value() ||
      (reading.room->phase == RoulettePhase::Result &&
       !reading.room->winningNumber.has_value())) {
    return {};
  }
  return wheelPictureAt(
      WheelMoment{.phase = reading.room->phase,
                  .winningNumber = reading.room->winningNumber.value_or(0),
                  .timeLeft = phaseTimeLeft(reading)},
      reading.motion);
}

/** @returns The number drawn @p reading shows, at the result only. */
std::optional<std::uint8_t> drawnOf(const RouletteReading& reading) {
  if (!reading.room.has_value() ||
      reading.room->phase != RoulettePhase::Result) {
    return std::nullopt;
  }
  return reading.room->winningNumber;
}

/** @returns The numbers drawn @p reading shows, newest first,
 * LAST_NUMBERS_KEPT at most: the one drawn at the result, then those of the
 * rounds before. */
std::vector<std::uint8_t> lastNumbersOf(const RouletteReading& reading) {
  std::vector<std::uint8_t> numbers;
  if (const std::optional<std::uint8_t> drawn = drawnOf(reading)) {
    numbers.push_back(*drawn);
  }
  if (reading.room.has_value()) {
    numbers.insert(numbers.end(), reading.room->history.begin(),
                   reading.room->history.end());
  }
  numbers.resize(std::min(numbers.size(), LAST_NUMBERS_KEPT));
  return numbers;
}

/** @returns The name of the room of @p reading in capitals, as the tag of its
 * panel writes it; empty without a room. */
std::string roomNameOf(const RouletteReading& reading) {
  std::string name = reading.room.has_value() ? reading.room->name : "";
  std::ranges::transform(name, name.begin(), [](char letter) {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
  });
  return name;
}

}  // namespace

RouletteBarStates barStatesOf(const RouletteReading& reading) {
  const bool open = betsOpen(reading);
  return RouletteBarStates{
      .clearOpen = open && !reading.bets.empty(),
      .rebetOpen = open && !reading.previousBets.empty() &&
                   stakeOf(reading.previousBets) <= reading.chipsAtHand};
}

RouletteView rouletteViewOf(const RouletteReading& reading) {
  return RouletteView{
      .croupierInvites = betsOpen(reading),
      .wheel = wheelOf(reading),
      .bets = reading.bets,
      .cursor = betsOpen(reading) ? std::optional<ClothPlace>(reading.cursor)
                                  : std::nullopt,
      .drawn = drawnOf(reading),
      .roomName = roomNameOf(reading),
      .roster = rosterRowsOf(reading),
      .lastNumbers = lastNumbersOf(reading),
      .band = RouletteBand{
          .chips = chipsShownOf(reading),
          .stake = betsOpen(reading)
                       ? static_cast<std::uint32_t>(stakeOf(reading.bets))
                       : reading.acceptedStake,
          .status = statusLineOf(reading),
          .chosenChip = reading.chosenChip}};
}

}  // namespace rtype::client
