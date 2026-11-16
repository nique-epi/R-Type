#include "RecordingCasinoChannel.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

std::optional<rtype::network::CasinoRoomListing> RecordingCasinoChannel::rooms()
    const {
  return listedRooms;
}

void RecordingCasinoChannel::askRooms() { ++roomsAsks; }

void RecordingCasinoChannel::enter(std::uint8_t room) {
  enteredRooms.push_back(room);
}

void RecordingCasinoChannel::leave() { ++leaves; }

std::optional<rtype::network::CasinoRoomState> RecordingCasinoChannel::room()
    const {
  return roomState;
}

std::optional<rtype::client::CasinoRefusal>
RecordingCasinoChannel::seatRefusal() const {
  return refusedSeat;
}

void RecordingCasinoChannel::placeBets(
    std::uint64_t round, const std::vector<rtype::network::RouletteBet>& bets) {
  betRounds.push_back(round);
  sentBets.push_back(bets);
}

const std::optional<rtype::client::BetsReading>&
RecordingCasinoChannel::lastBets() const {
  return betsAnswer;
}

std::optional<rtype::network::RoundBets> RecordingCasinoChannel::ownBets(
    std::uint64_t round) const {
  if (!roundBets.has_value() || roundBets->round != round) {
    return std::nullopt;
  }
  return roundBets;
}
