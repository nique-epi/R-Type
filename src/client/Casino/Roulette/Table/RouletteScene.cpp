#include "RouletteScene.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ButlerPose.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ChipsAtHand.hpp"
#include "ClothPlace.hpp"
#include "MotionPreference.hpp"
#include "RouletteBand.hpp"
#include "RouletteBarStates.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
#include "RouletteRules.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteTexts.hpp"
#include "RouletteView.hpp"
#include "TimeConstants.hpp"
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

/** @returns The chips the master holds on the cloth for the bets of
 * @p reading it took; 0 before it took any. */
std::uint32_t acceptedStakeOf(const RouletteReading& reading) {
  return reading.accepted.has_value() ? reading.accepted->stake : 0;
}

/** @returns The chips the band shows for @p reading. */
std::uint32_t chipsShownOf(const RouletteReading& reading) {
  if (!reading.room.has_value() ||
      reading.room->phase == RoulettePhase::Result) {
    return reading.walletChips;
  }
  const std::uint32_t atHand =
      chipsAtHandOf(reading.walletChips, reading.accepted);
  if (reading.room->phase == RoulettePhase::Spinning) {
    const std::uint32_t staked = acceptedStakeOf(reading);
    return atHand > staked ? atHand - staked : 0;
  }
  const std::uint64_t stake = network::stakeOf(reading.bets);
  return atHand > stake ? static_cast<std::uint32_t>(atHand - stake) : 0;
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

/** @returns The frame @p frame of @p row of the sheet of the butler, cut at
 * CROUPIER_HEIGHT. */
sf::IntRect croupierFrame(int row, int frame) {
  return {{frame * BUTLER_FRAME_SIZE.x, row * BUTLER_FRAME_SIZE.y},
          {BUTLER_FRAME_SIZE.x, CROUPIER_HEIGHT}};
}

/** @returns The frame of the croupier at the time of the master of
 * @p reading: idle, round and round, and, while the bets are open, his
 * greeting at the end of each CROUPIER_INVITE_CYCLE; the frames of less
 * motion, or before the first state of the room, held still. */
sf::IntRect croupierFrameOf(const RouletteReading& reading) {
  if (reading.motion == MotionPreference::Reduced ||
      !reading.room.has_value()) {
    return betsOpen(reading) ? CROUPIER_INVITING_FRAME : CROUPIER_WAITING_FRAME;
  }
  const engine::Duration now = reading.room->serverTime.time_since_epoch();
  const engine::Duration intoInvite = now % CROUPIER_INVITE_CYCLE;
  if (betsOpen(reading) && intoInvite >= CROUPIER_INVITE_AFTER) {
    const auto frame =
        (intoInvite - CROUPIER_INVITE_AFTER) / BUTLER_GESTURE_FRAME_TIME;
    return croupierFrame(static_cast<int>(ButlerPose::Greeting),
                         static_cast<int>(std::min<engine::Duration::rep>(
                             frame, BUTLER_FRAMES_PER_POSE - 1)));
  }
  return croupierFrame(static_cast<int>(ButlerPose::Idle),
                       static_cast<int>((now / BUTLER_IDLE_FRAME_TIME) %
                                        BUTLER_FRAMES_PER_POSE));
}

}  // namespace

RouletteBarStates barStatesOf(const RouletteReading& reading) {
  const bool open = betsOpen(reading);
  return RouletteBarStates{
      .clearOpen = open && !reading.bets.empty(),
      .rebetOpen = open && !reading.previousBets.empty() &&
                   network::stakeOf(reading.previousBets) <=
                       chipsAtHandOf(reading.walletChips, reading.accepted)};
}

RouletteView rouletteViewOf(const RouletteReading& reading) {
  return RouletteView{
      .croupier = croupierFrameOf(reading),
      .wheel = wheelOf(reading),
      .bets = reading.bets,
      .cursor = betsOpen(reading) ? std::optional<ClothPlace>(reading.cursor)
                                  : std::nullopt,
      .drawn = drawnOf(reading),
      .roomName = roomNameOf(reading),
      .roster = rosterRowsOf(reading),
      .lastNumbers = lastNumbersOf(reading),
      .band = RouletteBand{.chips = chipsShownOf(reading),
                           .stake = betsOpen(reading)
                                        ? static_cast<std::uint32_t>(
                                              network::stakeOf(reading.bets))
                                        : acceptedStakeOf(reading),
                           .status = statusLineOf(reading),
                           .chosenChip = reading.chosenChip}};
}

}  // namespace rtype::client
