#include "CasinoBetsDesk.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "IAccountSession.hpp"
#include "ICasinoLink.hpp"
#include "IWallet.hpp"
#include "OwnBetsReading.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBets.hpp"
#include "RoundBets.hpp"

namespace rtype::client {

CasinoBetsDesk::CasinoBetsDesk(ICasinoLink& link,
                               const IAccountSession& session, IWallet& wallet)
    : link_(&link), session_(&session), wallet_(&wallet) {}

void CasinoBetsDesk::place(network::RoomRound round,
                           const std::vector<network::RouletteBet>& bets) {
  if (!session_->signedIn()) {
    betsAsk_ = 0;
    lastBets_ = BetsReading{.ask = 0,
                            .round = round,
                            .receipt = std::nullopt,
                            .refusal = CasinoRefusal::SignedOut};
    return;
  }
  betsAsk_ = link_->askBets(session_->token(), round,
                            network::RouletteBets{.bets = bets});
}

void CasinoBetsDesk::follow(
    const CasinoReadings& readings,
    const std::optional<network::CasinoRoomState>& room) {
  settleBets(readings.bets);
  askOwnBets(room);
  settleOwnBets(readings.ownBets);
}

const std::optional<BetsReading>& CasinoBetsDesk::lastBets() const {
  return lastBets_;
}

std::optional<network::RoundBets> CasinoBetsDesk::ownBets(
    std::uint64_t round) const {
  if (!ownBets_.has_value() || ownBets_->round != round) {
    return std::nullopt;
  }
  return ownBets_;
}

void CasinoBetsDesk::forget() {
  betsAsk_ = 0;
  lastBets_.reset();
  betRound_.reset();
  readRound_.reset();
  ownBetsAsk_ = 0;
  ownBets_.reset();
}

void CasinoBetsDesk::settleBets(const std::optional<BetsReading>& reading) {
  if (betsAsk_ == 0 || !reading.has_value() || reading->ask != betsAsk_) {
    return;
  }
  betsAsk_ = 0;
  lastBets_ = reading;
  if (reading->receipt.has_value()) {
    betRound_ = reading->round;
    wallet_->askWallet();
  }
}

void CasinoBetsDesk::askOwnBets(
    const std::optional<network::CasinoRoomState>& room) {
  if (!betRound_.has_value() || readRound_ == betRound_ || !room.has_value() ||
      room->id != betRound_->room || room->round != betRound_->round ||
      !room->winningNumber.has_value()) {
    return;
  }
  readRound_ = betRound_;
  ownBetsAsk_ = link_->askOwnBets(session_->token(), *betRound_);
}

void CasinoBetsDesk::settleOwnBets(
    const std::optional<OwnBetsReading>& reading) {
  if (ownBetsAsk_ == 0 || !reading.has_value() || reading->ask != ownBetsAsk_ ||
      !reading->bets.has_value()) {
    return;
  }
  ownBetsAsk_ = 0;
  ownBets_ = reading->bets;
  if (ownBets_->payout.value_or(0) > 0) {
    wallet_->askWallet();
  }
}

}  // namespace rtype::client
