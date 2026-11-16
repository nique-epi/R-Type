#include "RecordingCasinoLink.hpp"
#include <cstdint>
#include <string>
#include "CasinoReadings.hpp"
#include "CasinoSeat.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"

std::uint64_t RecordingCasinoLink::askRooms() { return ++roomsAsks; }

std::uint64_t RecordingCasinoLink::askRoom(std::uint8_t room) {
  roomsRead.push_back(room);
  return roomsRead.size();
}

std::uint64_t RecordingCasinoLink::askSeat(const std::string& token,
                                           rtype::client::CasinoSeat seat) {
  seatsAsked.push_back(seat);
  seatTokens.push_back(token);
  return seatsAsked.size();
}

std::uint64_t RecordingCasinoLink::askBets(
    [[maybe_unused]] const std::string& token, rtype::network::RoomRound round,
    [[maybe_unused]] const rtype::network::RouletteBets& bets) {
  betRounds.push_back(round);
  return betRounds.size();
}

std::uint64_t RecordingCasinoLink::askOwnBets(
    [[maybe_unused]] const std::string& token,
    rtype::network::RoomRound round) {
  ownBetRounds.push_back(round);
  return ownBetRounds.size();
}

rtype::client::CasinoReadings RecordingCasinoLink::readings() const {
  return answers;
}
