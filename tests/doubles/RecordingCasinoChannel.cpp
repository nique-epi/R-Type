#include "RecordingCasinoChannel.hpp"
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "CasinoPlace.hpp"
#include "ChipMoves.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"

void RecordingCasinoChannel::chooseTable(rtype::network::TablePlace place) {
  chosenPlaces_.push_back(place);
}

void RecordingCasinoChannel::placeBets(
    [[maybe_unused]] std::vector<rtype::network::RouletteBet> bets,
    [[maybe_unused]] bool spinVote) {
  ++betsLaid_;
}

std::optional<rtype::network::RouletteRound> RecordingCasinoChannel::round()
    const {
  return round_;
}

rtype::client::ChipMoves RecordingCasinoChannel::takeChipMoves() {
  return std::exchange(chipMoves_, {});
}

void RecordingCasinoChannel::setRound(
    std::optional<rtype::network::RouletteRound> round) {
  round_ = std::move(round);
}

void RecordingCasinoChannel::setChipMoves(rtype::client::ChipMoves moves) {
  chipMoves_ = moves;
}

const std::vector<rtype::network::TablePlace>&
RecordingCasinoChannel::chosenPlaces() const {
  return chosenPlaces_;
}

std::size_t RecordingCasinoChannel::betsLaid() const { return betsLaid_; }
