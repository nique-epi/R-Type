#include "RouletteMemory.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "RouletteBets.hpp"
#include "RouletteMemoryConstants.hpp"
#include "RouletteRound.hpp"
#include "RouletteSlip.hpp"

namespace rtype::client {

void RouletteMemory::follow(const std::optional<network::RouletteRound>& round,
                            std::uint32_t chipsHeld) {
  if (!round.has_value()) {
    round_.reset();
    recordedRound_.reset();
    slip_.clear();
    spinAsked_ = false;
    return;
  }
  if (round_ != round->round) {
    startRound(round->round);
  }
  if (round->phase == network::RoulettePhase::Betting) {
    chipsBeforeDraw_ =
        chipsHeld > slip_.stake() ? chipsHeld - slip_.stake() : 0;
  } else if (round->phase == network::RoulettePhase::Result) {
    record(*round);
  }
}

RouletteSlip& RouletteMemory::slip() { return slip_; }

const RouletteSlip& RouletteMemory::slip() const { return slip_; }

const std::vector<network::RouletteBet>& RouletteMemory::previousBets() const {
  return previousBets_;
}

bool RouletteMemory::spinAsked() const { return spinAsked_; }

void RouletteMemory::askSpin() { spinAsked_ = true; }

const std::vector<std::uint8_t>& RouletteMemory::lastNumbers() const {
  return lastNumbers_;
}

std::optional<std::uint32_t> RouletteMemory::chipsBeforeDraw() const {
  return chipsBeforeDraw_;
}

void RouletteMemory::startRound(std::uint16_t number) {
  if (!slip_.bets().empty()) {
    previousBets_ = slip_.bets();
  }
  slip_.clear();
  spinAsked_ = false;
  round_ = number;
}

void RouletteMemory::record(const network::RouletteRound& round) {
  if (recordedRound_ == round.round) {
    return;
  }
  recordedRound_ = round.round;
  lastNumbers_.insert(lastNumbers_.begin(), round.winningNumber);
  if (lastNumbers_.size() > LAST_NUMBERS_KEPT) {
    lastNumbers_.resize(LAST_NUMBERS_KEPT);
  }
}

}  // namespace rtype::client
