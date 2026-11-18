#include "RouletteMemory.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "BetsReceipt.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"
#include "RouletteSlip.hpp"

namespace rtype::client {

void RouletteMemory::follow(const std::optional<network::CasinoRoomState>& room,
                            const std::optional<BetsReading>& bets) {
  if (!room.has_value()) {
    round_.reset();
    slip_.clear();
    accepted_.reset();
    return;
  }
  if (round_ != room->round) {
    startRound(room->round);
  }
  if (bets.has_value() && (bets->ask == 0 || bets->ask != readBetsAsk_)) {
    readBetsAsk_ = bets->ask;
    take(*bets);
  }
}

RouletteSlip& RouletteMemory::slip() { return slip_; }

const RouletteSlip& RouletteMemory::slip() const { return slip_; }

const std::vector<network::RouletteBet>& RouletteMemory::previousBets() const {
  return previousBets_;
}

const std::optional<network::BetsReceipt>& RouletteMemory::accepted() const {
  return accepted_;
}

void RouletteMemory::startRound(std::uint64_t round) {
  if (!slip_.bets().empty()) {
    previousBets_ = slip_.bets();
  }
  slip_.clear();
  accepted_.reset();
  round_ = round;
}

void RouletteMemory::take(const BetsReading& bets) {
  if (bets.round.round != round_) {
    return;
  }
  if (bets.receipt.has_value()) {
    accepted_ = bets.receipt;
    return;
  }
  slip_.clear();
  if (accepted_.has_value()) {
    static_cast<void>(slip_.replaceWith(accepted_->bets,
                                        accepted_->chips + accepted_->stake));
  }
}

}  // namespace rtype::client
