#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

namespace rtype::client {

/**
 * @brief The roulette rooms of the casino of the master, as the screens read
 * and play them: the master times the rounds, draws the numbers and settles
 * the chips; the client only reads and asks, each answer showing up on a
 * later frame.
 */
class ICasinoChannel {
 public:
  virtual ~ICasinoChannel() = default;

  /** @returns The newest rooms the master listed; nothing before the first
   * answer, or when the master did not answer the last ask. */
  [[nodiscard]] virtual std::optional<network::CasinoRoomListing> rooms()
      const = 0;

  /** @brief Asks the master the rooms again. */
  virtual void askRooms() = 0;

  /** @brief Follows @p room from now on, and seats the player there, the seat
   * kept as long as the room is followed; entering the room followed changes
   * nothing. Signed out, the room is followed and no seat taken. */
  virtual void enter(std::uint8_t room) = 0;

  /** @brief Gets the player up from the room followed, and follows none. */
  virtual void leave() = 0;

  /**
   * @returns The newest state of the room followed, timed on the clock of the
   * master as the client counts it: its serverTime is the clock of the master
   * now, the time it answered moved by the time the client counted since, and
   * its phase is the one its dates give at that time. Nothing before the
   * first answer, or without a room.
   */
  [[nodiscard]] virtual std::optional<network::CasinoRoomState> room()
      const = 0;

  /** @returns Why the master refused the seat in the room followed; nothing
   * once seated, or while it is asked. */
  [[nodiscard]] virtual std::optional<CasinoRefusal> seatRefusal() const = 0;

  /** @brief Lays @p bets on @p round of the room followed, in place of every
   * bet the player held there; none takes them all back. Ignored without a
   * room. */
  virtual void placeBets(std::uint64_t round,
                         const std::vector<network::RouletteBet>& bets) = 0;

  /** @returns The answer to the newest placeBets(): the receipt, or why the
   * master refused, at once SignedOut for a guest; nothing until it came. */
  [[nodiscard]] virtual std::optional<BetsReading> lastBets() const = 0;

  /** @returns The bets the player held on @p round of the room followed, and
   * what they paid, once the number of a round it bet on was drawn and the
   * master answered; nothing otherwise. */
  [[nodiscard]] virtual std::optional<network::RoundBets> ownBets(
      std::uint64_t round) const = 0;
};

}  // namespace rtype::client
