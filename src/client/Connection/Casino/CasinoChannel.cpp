#include "CasinoChannel.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "CasinoChannelConstants.hpp"
#include "CasinoPlace.hpp"
#include "ChipMoves.hpp"
#include "ISessionOutbox.hpp"
#include "IWaitingRoom.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "SequenceOrder.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The seat @p round lists for the player numbered @p number, if
 * any. */
const network::RouletteSeat* seatOf(const network::RouletteRound& round,
                                    std::uint8_t number) {
  const auto found = std::ranges::find(round.seats, number,
                                       &network::RouletteSeat::playerNumber);
  return found == round.seats.end() ? nullptr : &*found;
}

/** @returns Whether @p players show the player numbered @p number at the
 * casino. */
bool showsAtCasino(std::span<const network::LobbyPlayer> players,
                   std::uint8_t number) {
  return std::ranges::any_of(
      players, [number](const network::LobbyPlayer& player) {
        return player.playerNumber == number &&
               player.place == network::PilotPlace::AtCasino;
      });
}

/** @returns Whether @p received comes before @p kept: an older round, or an
 * earlier phase of the same round. */
bool isOlder(const network::RouletteRound& received,
             const network::RouletteRound& kept) {
  if (received.round != kept.round) {
    return network::isNewerSequence(kept.round, received.round);
  }
  return received.phase < kept.phase;
}

}  // namespace

CasinoChannel::CasinoChannel(network::MessageDispatcher& dispatcher,
                             const IWaitingRoom& room)
    : room_(&room) {
  dispatcher.registerHandler<network::RouletteRound>(
      [this](const network::MessageOrigin&,
             const network::RouletteRound& received) {
        if (outbox_ != nullptr && !room_->gameStarted()) {
          onRound(received);
        }
      });
}

void CasinoChannel::chooseTable(network::TablePlace place) {
  if (outbox_ == nullptr || room_->gameStarted() || requestedPlace_ == place) {
    return;
  }
  requestedPlace_ = place;
  if (place == network::TablePlace::InRoom) {
    bets_.reset();
  }
  sendMessage(*outbox_, network::CasinoPlace{.place = place});
  sinceSent_ = engine::Duration::zero();
}

void CasinoChannel::placeBets(std::vector<network::RouletteBet> bets,
                              bool spinVote) {
  if (outbox_ == nullptr || requestedPlace_ != network::TablePlace::AtTable ||
      !round_.has_value() || round_->phase != network::RoulettePhase::Betting) {
    return;
  }
  network::RouletteBets laid{
      .round = round_->round, .spinVote = spinVote, .bets = std::move(bets)};
  if (bets_ == laid) {
    return;
  }
  bets_ = std::move(laid);
  sendMessage(*outbox_, *bets_);
  sinceSent_ = engine::Duration::zero();
}

std::optional<network::RouletteRound> CasinoChannel::round() const {
  return round_;
}

ChipMoves CasinoChannel::takeChipMoves() { return std::exchange(moves_, {}); }

void CasinoChannel::sessionOpened(const network::LobbyPlayer& ownPlayer,
                                  ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownPlayerNumber_ = ownPlayer.playerNumber;
}

void CasinoChannel::sessionClosed() {
  outbox_ = nullptr;
  forgetTable();
  sinceSent_ = engine::Duration::zero();
}

void CasinoChannel::sessionHeld(engine::Duration elapsed) {
  if (outbox_ == nullptr) {
    return;
  }
  if (room_->gameStarted()) {
    forgetTable();
    return;
  }
  sinceSent_ += elapsed;
  if (sinceSent_ >= CASINO_MESSAGE_INTERVAL) {
    sendWhatIsPending();
    sinceSent_ = engine::Duration::zero();
  }
}

void CasinoChannel::onRound(const network::RouletteRound& received) {
  if (round_.has_value() && isOlder(received, *round_)) {
    return;
  }
  round_ = received;
  if (bets_.has_value() && bets_->round != received.round) {
    bets_.reset();
  }
  if (received.phase == network::RoulettePhase::Betting ||
      settledRound_ == received.round) {
    return;
  }
  settledRound_ = received.round;
  if (const network::RouletteSeat* const seat =
          seatOf(received, ownPlayerNumber_);
      seat != nullptr) {
    moves_.staked += seat->stake;
    moves_.won += seat->payout;
  }
}

void CasinoChannel::sendWhatIsPending() {
  const bool listed =
      round_.has_value() && seatOf(*round_, ownPlayerNumber_) != nullptr;
  if ((requestedPlace_ == network::TablePlace::AtTable && !listed) ||
      (requestedPlace_ == network::TablePlace::InRoom &&
       showsAtCasino(room_->lobbyPlayers(), ownPlayerNumber_))) {
    sendMessage(*outbox_, network::CasinoPlace{.place = *requestedPlace_});
  }
  if (bets_.has_value() && round_.has_value() &&
      round_->phase == network::RoulettePhase::Betting &&
      round_->round == bets_->round) {
    sendMessage(*outbox_, *bets_);
  }
}

void CasinoChannel::forgetTable() {
  requestedPlace_.reset();
  round_.reset();
  bets_.reset();
  settledRound_.reset();
}

}  // namespace rtype::client
