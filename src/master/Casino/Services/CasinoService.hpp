#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <functional>
#include <variant>
#include "BetOrder.hpp"
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "CasinoSettings.hpp"
#include "IAccountWallet.hpp"
#include "MasterDatabase.hpp"
#include "MillisecondDate.hpp"
#include "RoomRound.hpp"
#include "RoundBets.hpp"
#include "SeatKey.hpp"
#include "SeatOutcome.hpp"

namespace rtype::master {

/**
 * @brief The rules of the rooms of the casino: who sits where, the bets of a
 * round, and what the rooms show. The round and its phase are read on the
 * clock (scheduledRoundAt()); the number of a round is drawn by the
 * RouletteDealer once its bets close. A seat counts while it was seen in the
 * last network::CASINO_SEAT_LIFETIME.
 */
class CasinoService {
 public:
  /**
   * @param database It must outlive the service; so must @p wallet, which
   * moves the chips of the bets.
   * @param settings Gives the settings of the casino, read at each request.
   */
  CasinoService(MasterDatabase& database, IAccountWallet& wallet,
                std::function<CasinoSettings()> settings);

  /** @returns Whether the casino runs the room @p room. */
  [[nodiscard]] bool hasRoom(std::uint8_t room) const;

  /** @brief Seats the account of @p seat, or renews its seat, at @p now,
   * unless every seat of its room is held by others.
   * @returns What it did. */
  SeatOutcome takeSeat(SeatKey seat, std::chrono::sys_seconds now);

  /** @brief Gets the account of @p seat up; getting up again does nothing. */
  void leaveSeat(SeatKey seat);

  /**
   * @brief Replaces every bet the account of @p order holds on its round by
   * its bets, in one transaction: the bets it held are handed back (a refund),
   * the new ones staked. An empty list takes them all back.
   * @returns The bets, their stake and the chips left; or why nothing was
   * written: the round is not the one whose bets are open, or was drawn
   * already (BetsClosed); the account is not seated (NotSeated); a bet off the
   * cloth or of no chip, more than network::MAXIMUM_ROULETTE_BETS bets, more
   * than network::MAXIMUM_ROUND_STAKE chips, bets that could return more
   * than network::MAXIMUM_ROUND_PAYOUT, or more chips than the wallet holds
   * once the old bets are handed back.
   */
  [[nodiscard]] std::variant<network::BetsReceipt, BetRefusal> placeBets(
      const BetOrder& order);

  /** @returns The bets @p userId holds on @p place, and what they returned
   * once the round was drawn; no bets when it holds none. */
  [[nodiscard]] network::RoundBets betsOf(network::RoomRound place,
                                          std::int64_t userId);

  /** @returns Every room, with the players seated and the round at @p now. */
  [[nodiscard]] network::CasinoRoomListing rooms(network::MillisecondDate now);

  /** @returns The state of the room @p room at @p now: its number only once
   * its bets closed and it was drawn. */
  [[nodiscard]] network::CasinoRoomState roomState(
      std::uint8_t room, network::MillisecondDate now);

 private:
  /** @brief Writes the bets of @p order in place of the ones it replaces,
   * with one movement of the wallet for the difference of their stakes.
   * @returns The receipt; NotEnoughChips, with nothing written, when the
   * wallet holds less than the chips the new stake adds. */
  std::variant<network::BetsReceipt, BetRefusal> writtenBets(
      SQLite::Database& connection, const BetOrder& order);

  MasterDatabase& database_;
  IAccountWallet& wallet_;
  std::function<CasinoSettings()> settings_;
};

}  // namespace rtype::master
