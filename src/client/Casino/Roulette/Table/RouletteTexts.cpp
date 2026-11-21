#include "RouletteTexts.hpp"
#include <SFML/Graphics/Color.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "ChipsAtHand.hpp"
#include "Kopecks.hpp"
#include "MillisecondDate.hpp"
#include "PocketColors.hpp"
#include "RouletteColorConstants.hpp"
#include "RoulettePhase.hpp"
#include "RouletteReading.hpp"
#include "RouletteScreenConstants.hpp"
#include "RoundBets.hpp"
#include "TableRosterRow.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

using network::CasinoPlayer;
using network::RoulettePhase;

/** @returns What the status line says of @p refusal. */
std::string refusalLineOf(CasinoRefusal refusal) {
  switch (refusal) {
    case CasinoRefusal::BetsClosed:
      return BETS_CLOSED_LINE;
    case CasinoRefusal::NotSeated:
      return NOT_SEATED_LINE;
    case CasinoRefusal::RuleBroken:
      return RULE_BROKEN_LINE;
    case CasinoRefusal::NotEnoughChips:
      return NOT_ENOUGH_CHIPS_LINE;
    case CasinoRefusal::RoomFull:
      return ROOM_FULL_LINE;
    case CasinoRefusal::SignedOut:
      return SIGNED_OUT_LINE;
    case CasinoRefusal::MasterRefused:
      return CASINO_REFUSED_LINE;
    case CasinoRefusal::MasterUnreachable:
      break;
  }
  return CASINO_SILENT_LINE;
}

/** @returns What the status line says while the bets are open. */
std::string bettingLineOf(const RouletteReading& reading) {
  if (reading.refusal.has_value()) {
    return refusalLineOf(*reading.refusal);
  }
  if (chipsAtHandOf(reading.walletChips, reading.accepted) == 0 &&
      reading.bets.empty()) {
    return NO_CHIPS_LINE;
  }
  return BETS_CLOSE_IN_START +
         std::to_string(secondsBegunIn(phaseTimeLeft(reading)));
}

/** @returns What a stake of @p stake that paid @p payout won or lost, after
 * its sign; nothing for no chip laid. */
std::string gainOf(std::uint32_t stake, std::uint32_t payout) {
  if (payout > 0) {
    return GAIN_SIGN + kopecksText(payout);
  }
  if (stake > 0) {
    return LOSS_SIGN + kopecksText(stake);
  }
  return {};
}

/** @returns What the status line says at the result, @p number drawn. */
std::string resultLineOf(const RouletteReading& reading, std::uint8_t number) {
  std::string line =
      std::to_string(number) + RESULT_GAP + pocketColorWordOf(number);
  if (reading.ownBets.has_value() && reading.ownBets->payout.has_value()) {
    const std::string gain =
        gainOf(reading.ownBets->stake, *reading.ownBets->payout);
    if (!gain.empty()) {
      line += GAIN_GAP + gain;
    }
  }
  return line;
}

/** @returns The color the roster writes the value of @p player in: gold for
 * a stake not @p paid yet, green for a gain, pale blue for a loss. */
sf::Color valueColorOf(const CasinoPlayer& player, bool paid) {
  if (!paid) {
    return STAR_GOLD;
  }
  return player.payout.value_or(0) > 0 ? GAIN_GREEN : PALE_BLUE;
}

/** @returns Whether @p reading shows the result of its round, the wheel
 * stopped: the master sends the payouts as soon as it draws, while the wheel
 * still turns, and a gain shown then would give the number away. */
bool resultShown(const RouletteReading& reading) {
  return reading.room.has_value() &&
         reading.room->phase == RoulettePhase::Result;
}

/** @returns The row of @p player, seated at @p place in the room, from 0. */
TableRosterRow rowOf(const RouletteReading& reading, const CasinoPlayer& player,
                     std::size_t place) {
  const bool own = player.nickname == reading.ownNickname;
  const std::string gain = gainOf(player.stake, player.payout.value_or(0));
  const bool paid =
      resultShown(reading) && player.payout.has_value() && !gain.empty();
  return TableRosterRow{
      .colors = reading.palette.at(place % reading.palette.size()),
      .name = own ? player.nickname + OWN_NAME_END : player.nickname,
      .value = paid ? gain : kopecksText(player.stake),
      .valueColor = valueColorOf(player, paid)};
}

/** @returns The date the phase of @p room ends at: the close of the bets,
 * the result, then the end of the round. */
network::MillisecondDate phaseEndOf(const network::CasinoRoomState& room) {
  switch (room.phase) {
    case RoulettePhase::Betting:
      return room.closesAt;
    case RoulettePhase::Spinning:
      return room.resultAt;
    case RoulettePhase::Result:
      break;
  }
  return room.endsAt;
}

}  // namespace

engine::Duration phaseTimeLeft(const RouletteReading& reading) {
  if (!reading.room.has_value()) {
    return engine::Duration::zero();
  }
  const network::CasinoRoomState& room = *reading.room;
  const network::MillisecondDate end = phaseEndOf(room);
  return end > room.serverTime ? std::chrono::duration_cast<engine::Duration>(
                                     end - room.serverTime)
                               : engine::Duration::zero();
}

std::string statusLineOf(const RouletteReading& reading) {
  if (!reading.room.has_value()) {
    return TABLE_CLOSED_LINE;
  }
  switch (reading.room->phase) {
    case RoulettePhase::Betting:
      return bettingLineOf(reading);
    case RoulettePhase::Spinning:
      return RESULT_IN_START +
             std::to_string(secondsBegunIn(phaseTimeLeft(reading)));
    case RoulettePhase::Result:
      break;
  }
  if (!reading.room->winningNumber.has_value()) {
    return NO_MORE_BETS_LINE;
  }
  return resultLineOf(reading, *reading.room->winningNumber);
}

std::vector<TableRosterRow> rosterRowsOf(const RouletteReading& reading) {
  std::vector<TableRosterRow> own;
  std::vector<TableRosterRow> others;
  if (reading.room.has_value()) {
    const std::vector<CasinoPlayer>& players = reading.room->players;
    for (std::size_t place = 0; place < players.size(); ++place) {
      (players.at(place).nickname == reading.ownNickname ? own : others)
          .push_back(rowOf(reading, players.at(place), place));
    }
  }
  own.insert(own.end(), others.begin(), others.end());
  if (own.size() > ROSTER_MAXIMUM_ROWS) {
    own.resize(ROSTER_MAXIMUM_ROWS);
  }
  return own;
}

std::int64_t secondsBegunIn(engine::Duration timeLeft) {
  return static_cast<std::int64_t>(
      std::chrono::ceil<std::chrono::seconds>(timeLeft).count());
}

}  // namespace rtype::client
