#pragma once

#include <cstdint>
#include <string>
#include "CasinoReadings.hpp"
#include "CasinoSeat.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"

namespace rtype::client {

/**
 * @brief The calls to the casino of the master, without the frame ever
 * waiting on HTTP: an ask returns its number at once, and its reading shows
 * up on a later frame. A newer ask of a kind replaces the one before it,
 * unserved, so one call of a kind is in flight at most.
 *
 * The seats and the bets take the token of the session, which the link keeps
 * in memory only; the rooms are read without one.
 */
class ICasinoLink {
 public:
  virtual ~ICasinoLink() = default;

  /** @brief Asks the rooms of the casino; readings() answers it. */
  virtual void askRooms() = 0;

  /** @returns The number of this ask for the state of @p room. */
  virtual std::uint64_t askRoom(std::uint8_t room) = 0;

  /** @returns The number of this ask to put the account of @p token where
   * @p seat says: seated, which renews a seat held, or up. */
  virtual std::uint64_t askSeat(const std::string& token, CasinoSeat seat) = 0;

  /** @returns The number of this ask to lay @p bets on @p round for the
   * account of @p token, in place of every bet it held there. */
  virtual std::uint64_t askBets(const std::string& token,
                                network::RoomRound round,
                                const network::RouletteBets& bets) = 0;

  /** @returns The number of this ask for the bets of the account of
   * @p token on @p round, and what they paid. */
  virtual std::uint64_t askOwnBets(const std::string& token,
                                   network::RoomRound round) = 0;

  /** @returns The newest answer of each kind. */
  [[nodiscard]] virtual CasinoReadings readings() const = 0;
};

}  // namespace rtype::client
