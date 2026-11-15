#include "RouletteScene.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>
#include "ClothPlace.hpp"
#include "RouletteBand.hpp"
#include "RouletteBarStates.hpp"
#include "RouletteBets.hpp"
#include "RouletteReading.hpp"
#include "RouletteRound.hpp"
#include "RouletteTexts.hpp"
#include "RouletteView.hpp"
#include "TimeConstants.hpp"
#include "WheelMoment.hpp"
#include "WheelMotion.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

namespace {

using network::RoulettePhase;

/** @returns Whether the table of @p reading takes bets. */
bool betsOpen(const RouletteReading& reading) {
  return reading.round.has_value() &&
         reading.round->phase == RoulettePhase::Betting;
}

/** @returns The chips of @p bets added up. */
std::uint64_t stakeOf(const std::vector<network::RouletteBet>& bets) {
  std::uint64_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return stake;
}

/** @returns The chips of the bets of @p reading added up. */
std::uint32_t stakeLaidIn(const RouletteReading& reading) {
  return static_cast<std::uint32_t>(stakeOf(reading.bets));
}

/** @returns The chips the band shows for @p reading. */
std::uint32_t chipsShownOf(const RouletteReading& reading) {
  if (!reading.round.has_value() ||
      reading.round->phase == RoulettePhase::Result) {
    return reading.chipsHeld;
  }
  if (reading.round->phase == RoulettePhase::Spinning) {
    return reading.chipsBeforeDraw.value_or(reading.chipsHeld);
  }
  const std::uint32_t stake = stakeLaidIn(reading);
  return reading.chipsHeld > stake ? reading.chipsHeld - stake : 0;
}

/** @returns The stake the band shows for @p reading. */
std::uint32_t stakeShownOf(const RouletteReading& reading) {
  const std::optional<network::RouletteSeat> seat = ownSeatOf(reading);
  if (betsOpen(reading) || !seat.has_value()) {
    return stakeLaidIn(reading);
  }
  return seat->stake;
}

/** @returns What the wheel shows for @p reading. */
WheelPicture wheelOf(const RouletteReading& reading) {
  if (!reading.round.has_value()) {
    return {};
  }
  return wheelPictureAt(
      WheelMoment{.phase = reading.round->phase,
                  .winningNumber = reading.round->winningNumber,
                  .timeLeft = reading.timeLeft},
      reading.motion);
}

/** @returns The number drawn @p reading shows, at the result only. */
std::optional<std::uint8_t> drawnOf(const RouletteReading& reading) {
  if (!reading.round.has_value() ||
      reading.round->phase != RoulettePhase::Result) {
    return std::nullopt;
  }
  return reading.round->winningNumber;
}

}  // namespace

RouletteBarStates barStatesOf(const RouletteReading& reading) {
  const bool open = betsOpen(reading);
  return RouletteBarStates{
      .spinOpen = open && (reading.chipsHeld > 0 || !reading.bets.empty()),
      .spinAsked = open && reading.spinAsked,
      .clearOpen = open && !reading.bets.empty(),
      .rebetOpen = open && !reading.previousBets.empty() &&
                   stakeOf(reading.previousBets) <= reading.chipsHeld};
}

engine::Duration phaseTimeLeft(std::uint16_t ticksLeft,
                               engine::Duration since) {
  const engine::Duration phase =
      std::chrono::duration_cast<engine::Duration>(std::chrono::seconds(1)) *
      ticksLeft / engine::SIMULATION_TICKS_PER_SECOND;
  return phase > since ? phase - since : engine::Duration::zero();
}

RouletteView rouletteViewOf(const RouletteReading& reading) {
  return RouletteView{.croupierInvites = betsOpen(reading),
                      .wheel = wheelOf(reading),
                      .bets = reading.bets,
                      .cursor = betsOpen(reading)
                                    ? std::optional<ClothPlace>(reading.cursor)
                                    : std::nullopt,
                      .drawn = drawnOf(reading),
                      .roster = rosterRowsOf(reading),
                      .lastNumbers = reading.lastNumbers,
                      .band = RouletteBand{.chips = chipsShownOf(reading),
                                           .stake = stakeShownOf(reading),
                                           .status = statusLineOf(reading),
                                           .chosenChip = reading.chosenChip}};
}

}  // namespace rtype::client
