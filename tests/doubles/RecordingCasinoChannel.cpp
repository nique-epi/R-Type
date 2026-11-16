#include "RecordingCasinoChannel.hpp"
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "ChipMoves.hpp"
#include "RouletteBet.hpp"
#include "RouletteRound.hpp"
#include "TablePlace.hpp"

void RecordingCasinoChannel::chooseTable(rtype::network::TablePlace place) {
  chosenPlaces_.push_back(place);
}

void RecordingCasinoChannel::placeBets(
    std::vector<rtype::network::RouletteBet> bets, bool spinVote) {
  ++betsLaid_;
  sentBets_.push_back(std::move(bets));
  spinVotes_.push_back(spinVote);
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

const std::vector<std::vector<rtype::network::RouletteBet>>&
RecordingCasinoChannel::sentBets() const {
  return sentBets_;
}

const std::vector<bool>& RecordingCasinoChannel::spinVotes() const {
  return spinVotes_;
}
