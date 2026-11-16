#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "ICasinoChannel.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

/**
 * @brief Casino double that writes down every room entered, every leave, every
 * ask for the rooms and every set of bets placed, in order, and whose rooms,
 * room, refusal and answers the test sets: a real casino changes them on its
 * own, at any time between two updates. Public so a test sets and reads them.
 */
class RecordingCasinoChannel final : public rtype::client::ICasinoChannel {
 public:
  [[nodiscard]] std::optional<rtype::network::CasinoRoomListing> rooms()
      const override;
  /** @brief Counts the ask. */
  void askRooms() override;
  /** @brief Writes the room down. */
  void enter(std::uint8_t room) override;
  /** @brief Counts the leave. */
  void leave() override;
  [[nodiscard]] std::optional<rtype::network::CasinoRoomState> room()
      const override;
  [[nodiscard]] std::optional<rtype::client::CasinoRefusal> seatRefusal()
      const override;
  /** @brief Writes the round and the bets down. */
  void placeBets(std::uint64_t round,
                 const std::vector<rtype::network::RouletteBet>& bets) override;
  [[nodiscard]] const std::optional<rtype::client::BetsReading>& lastBets()
      const override;
  [[nodiscard]] std::optional<rtype::network::RoundBets> ownBets(
      std::uint64_t round) const override;

  std::optional<rtype::network::CasinoRoomListing> listedRooms;
  std::optional<rtype::network::CasinoRoomState> roomState;
  std::optional<rtype::client::CasinoRefusal> refusedSeat;
  std::optional<rtype::client::BetsReading> betsAnswer;
  std::optional<rtype::network::RoundBets> roundBets;

  std::vector<std::uint8_t> enteredRooms;
  std::size_t leaves{};
  std::size_t roomsAsks{};
  std::vector<std::uint64_t> betRounds;
  std::vector<std::vector<rtype::network::RouletteBet>> sentBets;
};
