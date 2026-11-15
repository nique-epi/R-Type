#include "RouletteTexts.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "Kopecks.hpp"
#include "LobbyState.hpp"
#include "PocketColors.hpp"
#include "RouletteColorConstants.hpp"
#include "RouletteReading.hpp"
#include "RouletteRound.hpp"
#include "RouletteScreenConstants.hpp"
#include "TableRosterRow.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

using network::RoulettePhase;
using network::RouletteSeat;

/** @returns The whole seconds of @p timeLeft, a second begun counted. */
std::int64_t secondsBegunIn(engine::Duration timeLeft) {
  return static_cast<std::int64_t>(
      std::chrono::ceil<std::chrono::seconds>(timeLeft).count());
}

/** @returns What the status line says while the bets are open. */
std::string bettingLineOf(const RouletteReading& reading) {
  if (reading.chipsHeld == 0 && reading.bets.empty()) {
    return NO_CHIPS_LINE;
  }
  return BETS_CLOSE_IN_START + std::to_string(secondsBegunIn(reading.timeLeft));
}

/** @returns What @p seat won or lost at the result, after its sign; nothing
 * for a seat that laid no chip. */
std::string gainOf(const RouletteSeat& seat) {
  if (seat.payout > 0) {
    return GAIN_SIGN + kopecksText(seat.payout);
  }
  if (seat.stake > 0) {
    return LOSS_SIGN + kopecksText(seat.stake);
  }
  return {};
}

/** @returns What the status line says at the result of @p round. */
std::string resultLineOf(const RouletteReading& reading,
                         const network::RouletteRound& round) {
  std::string line = std::to_string(round.winningNumber) + RESULT_GAP +
                     pocketColorWordOf(round.winningNumber);
  const std::optional<RouletteSeat> seat = ownSeatOf(reading);
  if (seat.has_value() && !gainOf(*seat).empty()) {
    line += GAIN_GAP + gainOf(*seat);
  }
  return line;
}

/** @returns The name of @p playerNumber in the waiting room of @p reading,
 * followed by (YOU) for the player. */
std::string nameOf(const RouletteReading& reading, std::uint8_t playerNumber) {
  const auto player = std::ranges::find(reading.players, playerNumber,
                                        &network::LobbyPlayer::playerNumber);
  const std::string name =
      player == reading.players.end()
          ? UNNAMED_PILOT_START + std::to_string(playerNumber)
          : player->nickname;
  return playerNumber == reading.ownPlayerNumber ? name + OWN_NAME_END : name;
}

/** @returns The color the value of @p seat is written in: gold before the
 * result, then green for a gain and pale blue otherwise. */
sf::Color valueColorOf(const RouletteSeat& seat, bool result) {
  if (!result) {
    return STAR_GOLD;
  }
  return seat.payout > 0 ? GAIN_GREEN : PALE_BLUE;
}

/** @returns The phase of the table of @p reading, which has one. */
RoulettePhase phaseOf(const RouletteReading& reading) {
  return reading.round.has_value() ? reading.round->phase
                                   : RoulettePhase::Betting;
}

/** @returns The row of @p seat, its number one of the player colors. */
TableRosterRow rowOf(const RouletteReading& reading, const RouletteSeat& seat) {
  const bool result = phaseOf(reading) == RoulettePhase::Result;
  const bool own = seat.playerNumber == reading.ownPlayerNumber;
  return TableRosterRow{
      .colors =
          reading.palette.at(static_cast<std::size_t>(seat.playerNumber) - 1),
      .name = nameOf(reading, seat.playerNumber),
      .spinAsked = own && reading.spinAsked &&
                   phaseOf(reading) == RoulettePhase::Betting,
      .value = result && !gainOf(seat).empty() ? gainOf(seat)
                                               : kopecksText(seat.stake),
      .valueColor = valueColorOf(seat, result)};
}

}  // namespace

std::optional<RouletteSeat> ownSeatOf(const RouletteReading& reading) {
  if (!reading.round.has_value()) {
    return std::nullopt;
  }
  const auto seat =
      std::ranges::find(reading.round->seats, reading.ownPlayerNumber,
                        &RouletteSeat::playerNumber);
  if (seat == reading.round->seats.end()) {
    return std::nullopt;
  }
  return *seat;
}

std::string statusLineOf(const RouletteReading& reading) {
  if (!reading.round.has_value()) {
    return TABLE_CLOSED_LINE;
  }
  switch (reading.round->phase) {
    case RoulettePhase::Betting:
      return bettingLineOf(reading);
    case RoulettePhase::Spinning:
      return NO_MORE_BETS_LINE;
    case RoulettePhase::Result:
      break;
  }
  return resultLineOf(reading, *reading.round);
}

std::vector<TableRosterRow> rosterRowsOf(const RouletteReading& reading) {
  std::vector<TableRosterRow> rows;
  if (!reading.round.has_value()) {
    return rows;
  }
  for (const RouletteSeat& seat : reading.round->seats) {
    if (seat.playerNumber >= 1 && seat.playerNumber <= PLAYER_COLOR_COUNT) {
      rows.push_back(rowOf(reading, seat));
    }
  }
  return rows;
}

}  // namespace rtype::client
