#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "CasinoReadings.hpp"
#include "CasinoSeat.hpp"
#include "ICasinoLink.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"

/**
 * @brief Casino link double that writes down every ask, in order, numbers the
 * asks of each kind from 1 as the real link does, and hands over the readings
 * the test sets, as the thread of the real link publishes them on a later
 * frame. Public so a test sets and reads them.
 */
class RecordingCasinoLink final : public rtype::client::ICasinoLink {
 public:
  void askRooms() override;
  std::uint64_t askRoom(std::uint8_t room) override;
  std::uint64_t askSeat(const std::string& token,
                        rtype::client::CasinoSeat seat) override;
  std::uint64_t askBets(const std::string& token,
                        rtype::network::RoomRound round,
                        const rtype::network::RouletteBets& bets) override;
  std::uint64_t askOwnBets(const std::string& token,
                           rtype::network::RoomRound round) override;
  [[nodiscard]] rtype::client::CasinoReadings readings() const override;
  /** @returns A new number at each call, since a test sets the answers
   * without telling it: a reader copies them every time. */
  [[nodiscard]] std::uint64_t revision() const override;

  rtype::client::CasinoReadings answers;
  /** @brief How many times revision() was called. */
  mutable std::uint64_t revisionsRead{};

  std::uint64_t roomsAsks{};
  std::vector<std::uint8_t> roomsRead;
  std::vector<rtype::client::CasinoSeat> seatsAsked;
  std::vector<std::string> seatTokens;
  std::vector<rtype::network::RoomRound> betRounds;
  std::vector<rtype::network::RoomRound> ownBetRounds;
};
