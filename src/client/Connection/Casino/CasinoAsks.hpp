#pragma once

#include <cstdint>
#include <string>
#include "CasinoSeat.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"

namespace rtype::client {

/**
 * @brief What the frame loop asked of the casino so far, as MasterCasinoLink
 * hands it to its thread: each ask has a number one above the previous one of
 * its kind, so the thread serves the newest ask only, and a reading tells
 * which ask it answers. 0 means not asked yet.
 *
 * The token is kept in memory only: never logged, never written.
 */
struct CasinoAsks {
  std::uint64_t roomsAsk{};
  std::uint64_t roomAsk{};
  /** @brief The room roomAsk reads. */
  std::uint8_t room{};
  std::uint64_t seatAsk{};
  /** @brief Where seatAsk puts the player. */
  CasinoSeat seat;
  std::uint64_t betsAsk{};
  /** @brief The round betsAsk lays bets on, and the bets. */
  network::RoomRound betsRound;
  network::RouletteBets bets;
  std::uint64_t ownBetsAsk{};
  /** @brief The round ownBetsAsk reads the bets of the player on. */
  network::RoomRound ownBetsRound;
  /** @brief The token the newest ask for a seat or bets was made with. */
  std::string token;
};

}  // namespace rtype::client
