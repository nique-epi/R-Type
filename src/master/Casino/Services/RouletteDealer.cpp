#include "RouletteDealer.hpp"
#include <SQLiteCpp/Database.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <vector>
#include "CasinoBetRepository.hpp"
#include "CasinoRoomConstants.hpp"
#include "CasinoRoundRepository.hpp"
#include "CasinoSeatRepository.hpp"
#include "CasinoServiceConstants.hpp"
#include "CasinoSettings.hpp"
#include "ChipMovement.hpp"
#include "DrawnRound.hpp"
#include "IAccountWallet.hpp"
#include "IRouletteDraw.hpp"
#include "MasterDatabase.hpp"
#include "MillisecondDate.hpp"
#include "MovementReason.hpp"
#include "RoomRound.hpp"
#include "RoulettePhase.hpp"
#include "RouletteSchedule.hpp"
#include "RoundPayouts.hpp"
#include "RoundReference.hpp"
#include "ScheduledRound.hpp"

namespace rtype::master {

namespace {

/** @returns The round of each of the first @p rooms rooms whose bets closed
 * at @p scheduled, and that @p connection holds no number of: one query for
 * every room. */
std::vector<network::RoomRound> closedRoundsOf(SQLite::Database& connection,
                                               const ScheduledRound& scheduled,
                                               std::uint8_t rooms) {
  std::vector<network::RoomRound> closed;
  if (scheduled.phase == network::RoulettePhase::Betting) {
    return closed;
  }
  const std::vector<std::uint8_t> drawn =
      CasinoRoundRepository{connection}.roomsDrawnIn(scheduled.round);
  for (std::uint8_t offset = 0; offset < rooms; ++offset) {
    const auto room =
        static_cast<std::uint8_t>(network::FIRST_CASINO_ROOM + offset);
    if (std::ranges::find(drawn, room) == drawn.end()) {
      closed.push_back(
          network::RoomRound{.room = room, .round = scheduled.round});
    }
  }
  return closed;
}

}  // namespace

RouletteDealer::RouletteDealer(MasterDatabase& database, IAccountWallet& wallet,
                               network::IRouletteDraw& draw)
    : database_(database), wallet_(wallet), draw_(draw) {}

std::vector<DrawnRound> RouletteDealer::drawDueRounds(
    network::MillisecondDate now, const CasinoSettings& settings) {
  const ScheduledRound scheduled = scheduledRoundAt(now, settings.durations);
  return drawnEach(
      database_.run([&scheduled, &settings](SQLite::Database& connection) {
        return closedRoundsOf(connection, scheduled, settings.rooms);
      }),
      now);
}

std::vector<DrawnRound> RouletteDealer::drawMissedRounds(
    network::MillisecondDate now, const CasinoSettings& settings) {
  const ScheduledRound scheduled = scheduledRoundAt(now, settings.durations);
  return drawnEach(
      database_.run([&scheduled, &settings](SQLite::Database& connection) {
        std::vector<network::RoomRound> due =
            CasinoBetRepository{connection}.undrawnBefore(
                firstOpenRound(scheduled));
        for (const network::RoomRound& place :
             closedRoundsOf(connection, scheduled, settings.rooms)) {
          if (std::ranges::find(due, place) == due.end()) {
            due.push_back(place);
          }
        }
        return due;
      }),
      now);
}

std::vector<DrawnRound> RouletteDealer::drawnEach(
    const std::vector<network::RoomRound>& places,
    network::MillisecondDate now) {
  const auto second = std::chrono::floor<std::chrono::seconds>(now);
  std::vector<DrawnRound> drawnRounds;
  for (const network::RoomRound& place : places) {
    if (const std::optional<DrawnRound> round = drawn(place, second)) {
      drawnRounds.push_back(*round);
    }
  }
  if (!drawnRounds.empty()) {
    tidy(second);
  }
  return drawnRounds;
}

std::optional<DrawnRound> RouletteDealer::drawn(network::RoomRound place,
                                                std::chrono::sys_seconds now) {
  return database_.transact([this, place, now](SQLite::Database& connection)
                                -> std::optional<DrawnRound> {
    CasinoRoundRepository rounds{connection};
    if (rounds.numberOf(place).has_value()) {
      return std::nullopt;
    }
    const DrawnRound round{
        .place = place, .winningNumber = draw_.drawNumber(), .drawnAt = now};
    rounds.insert(round);
    for (const auto& [userId, payout] :
         payoutsOf(CasinoBetRepository{connection}.roundBetsOf(place),
                   round.winningNumber)) {
      if (payout > 0) {
        static_cast<void>(wallet_.applyChips(
            connection, ChipMovement{.userId = userId,
                                     .amount = payout,
                                     .reason = network::MovementReason::Payout,
                                     .reference = roundReferenceOf(place),
                                     .madeAt = now}));
      }
    }
    return round;
  });
}

void RouletteDealer::tidy(std::chrono::sys_seconds now) {
  database_.transact([now](SQLite::Database& connection) {
    CasinoSeatRepository{connection}.removeSeenBefore(
        now - network::CASINO_SEAT_LIFETIME);
    CasinoBetRepository{connection}.removeDrawnBefore(now -
                                                      DRAWN_BETS_KEPT_FOR);
    CasinoRoundRepository{connection}.pruneKeeping(
        network::CASINO_HISTORY_LENGTH);
  });
}

}  // namespace rtype::master
