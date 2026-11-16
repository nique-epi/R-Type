#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoReadings.hpp"
#include "CasinoRoomState.hpp"
#include "IAccountSession.hpp"
#include "ICasinoLink.hpp"
#include "IWallet.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

namespace rtype::client {

/**
 * @brief The bets of the player of an IAccountSession on the rounds of a room,
 * laid through an ICasinoLink, and what they paid.
 *
 * The chips are the master's: the wallet is asked again once bets were taken,
 * since the master staked them, and once a round they were taken on paid
 * something. The bets of a round are read once its number shows in the state
 * of the room, for the newest round bets were taken on only.
 */
class CasinoBetsDesk {
 public:
  /** @param link The calls to the master, @p session the player and
   * @p wallet its chips; all must outlive the desk. */
  CasinoBetsDesk(ICasinoLink& link, const IAccountSession& session,
                 IWallet& wallet);

  /** @brief Lays @p bets on @p round, in place of the bets held there; a
   * guest is answered SignedOut at once. */
  void place(network::RoomRound round,
             const std::vector<network::RouletteBet>& bets);

  /** @brief Reads the answers of @p readings, and asks the bets of the round
   * bets were taken on once @p room shows its number. Called once per
   * frame. */
  void follow(const CasinoReadings& readings,
              const std::optional<network::CasinoRoomState>& room);

  /** @returns The answer to the newest place(); nothing until it came. */
  [[nodiscard]] std::optional<BetsReading> lastBets() const;

  /** @returns The bets read on @p round; nothing until they were. */
  [[nodiscard]] std::optional<network::RoundBets> ownBets(
      std::uint64_t round) const;

  /** @brief Forgets the bets of the room left. */
  void forget();

 private:
  /** @brief Keeps the answer to the newest bets, and asks the wallet again
   * once they were taken. */
  void settleBets(const std::optional<BetsReading>& reading);
  /** @brief Asks the bets of the round bets were taken on once @p room shows
   * its number. */
  void askOwnBets(const std::optional<network::CasinoRoomState>& room);
  /** @brief Keeps the bets read, and asks the wallet again once they paid. */
  void settleOwnBets(const std::optional<OwnBetsReading>& reading);

  ICasinoLink* link_;
  const IAccountSession* session_;
  IWallet* wallet_;
  std::uint64_t betsAsk_{};
  std::optional<BetsReading> lastBets_;
  /** @brief The newest round bets were taken on, and the round whose bets
   * were asked. */
  std::optional<network::RoomRound> betRound_;
  std::optional<network::RoomRound> readRound_;
  std::uint64_t ownBetsAsk_{};
  std::optional<network::RoundBets> ownBets_;
};

}  // namespace rtype::client
