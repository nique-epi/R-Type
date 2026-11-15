#include "RouletteTable.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include "CasinoConstants.hpp"
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "IRouletteDraw.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "RouletteRules.hpp"
#include "SeatIndex.hpp"
#include "TableSeat.hpp"

namespace rtype::server {

namespace {

/** @returns The chips of @p bets added up, wide enough never to wrap. */
std::uint64_t stakeOf(const network::RouletteBets& bets) {
  std::uint64_t stake = 0;
  for (const network::RouletteBet& bet : bets.bets) {
    stake += bet.amount;
  }
  return stake;
}

/** @returns Whether @p bets may lie on the cloth for one round. */
bool isTakeable(const network::RouletteBets& bets) {
  return std::ranges::all_of(bets.bets, isPlaceableBet) &&
         stakeOf(bets) <= MAXIMUM_ROUND_STAKE;
}

/** @returns What the bets of @p seat return when @p drawn comes out. */
std::uint32_t payoutOfSeat(const TableSeat& seat, std::uint8_t drawn) {
  std::uint64_t payout = 0;
  for (const network::RouletteBet& bet : seat.bets) {
    payout += payoutOf(bet, drawn);
  }
  return static_cast<std::uint32_t>(payout);
}

}  // namespace

RouletteTable::RouletteTable(PlayerRoster& roster, IRouletteDraw& draw)
    : roster_(roster), draw_(draw), ticksLeft_(ROULETTE_BETTING_TICKS) {}

void RouletteTable::onPlace(ConnectedPlayer& player,
                            network::TablePlace place) {
  TableSeat& seat = seats_.at(seatIndexOf(player));
  const bool sits = place == network::TablePlace::AtTable;
  player.atCasino = sits;
  if (sits) {
    player.ready = false;
    seat.seated = true;
    seat.listed = true;
    return;
  }
  seat.seated = false;
  if (phase_ == network::RoulettePhase::Betting) {
    seat = TableSeat{};
  }
}

void RouletteTable::onBets(const ConnectedPlayer& player,
                           const network::RouletteBets& bets) {
  TableSeat& seat = seats_.at(seatIndexOf(player));
  if (!seat.seated || phase_ != network::RoulettePhase::Betting ||
      bets.round != round_ || !isTakeable(bets)) {
    return;
  }
  seat.bets = bets.bets;
  seat.stake = static_cast<std::uint32_t>(stakeOf(bets));
  seat.spinVote = bets.spinVote;
}

void RouletteTable::onPlayerClosing(const ConnectedPlayer& departed) {
  seats_.at(seatIndexOf(departed)) = TableSeat{};
}

void RouletteTable::onTick() {
  switch (phase_) {
    case network::RoulettePhase::Betting:
      if (!hasSeatedPlayer()) {
        ticksLeft_ = ROULETTE_BETTING_TICKS;
      } else if (everySeatedPlayerVoted() || --ticksLeft_ == 0) {
        spin();
      }
      return;
    case network::RoulettePhase::Spinning:
      if (--ticksLeft_ == 0) {
        phase_ = network::RoulettePhase::Result;
        ticksLeft_ = ROULETTE_RESULT_TICKS;
      }
      return;
    case network::RoulettePhase::Result:
      if (--ticksLeft_ == 0) {
        openNextRound();
      }
      return;
  }
}

void RouletteTable::close() {
  for (ConnectedPlayer* const player : roster_.connected()) {
    player->atCasino = false;
  }
  seats_ = {};
  round_ = 0;
  phase_ = network::RoulettePhase::Betting;
  ticksLeft_ = ROULETTE_BETTING_TICKS;
  winningNumber_ = 0;
}

network::RouletteRound RouletteTable::round() const {
  network::RouletteRound state{
      .round = round_,
      .phase = phase_,
      .ticksLeft = static_cast<std::uint16_t>(ticksLeft_),
      .winningNumber = winningNumber_,
      .seats = {}};
  for (std::size_t index = 0; index < seats_.size(); ++index) {
    const TableSeat& seat = seats_.at(index);
    if (seat.listed) {
      state.seats.push_back(network::RouletteSeat{
          .playerNumber =
              static_cast<std::uint8_t>(network::FIRST_PLAYER_NUMBER + index),
          .stake = seat.stake,
          .payout = seat.payout});
    }
  }
  return state;
}

void RouletteTable::spin() {
  winningNumber_ = draw_.drawNumber();
  for (TableSeat& seat : seats_) {
    seat.payout = seat.listed ? payoutOfSeat(seat, winningNumber_) : 0;
  }
  phase_ = network::RoulettePhase::Spinning;
  ticksLeft_ = ROULETTE_SPINNING_TICKS;
}

void RouletteTable::openNextRound() {
  ++round_;
  phase_ = network::RoulettePhase::Betting;
  ticksLeft_ = ROULETTE_BETTING_TICKS;
  for (TableSeat& seat : seats_) {
    seat = TableSeat{.seated = seat.seated,
                     .listed = seat.seated,
                     .spinVote = false,
                     .bets = {},
                     .stake = 0,
                     .payout = 0};
  }
}

bool RouletteTable::everySeatedPlayerVoted() const {
  return hasSeatedPlayer() &&
         std::ranges::all_of(seats_, [](const TableSeat& seat) {
           return !seat.seated || seat.spinVote;
         });
}

bool RouletteTable::hasSeatedPlayer() const {
  return std::ranges::any_of(seats_, &TableSeat::seated);
}

}  // namespace rtype::server
