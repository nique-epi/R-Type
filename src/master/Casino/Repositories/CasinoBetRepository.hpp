#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <vector>
#include "PlayerBet.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"

namespace rtype::master {

/** @brief The table `casino_bets`: the only code that writes its SQL. It works
 * on a connection the caller holds, inside the caller's transaction. */
class CasinoBetRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit CasinoBetRepository(SQLite::Database& connection);

  /** @returns The bets @p userId holds on @p place, in the order it sent
   * them. */
  [[nodiscard]] std::vector<network::RouletteBet> betsOf(
      network::RoomRound place, std::int64_t userId);

  /** @returns Every bet of @p place, by account. */
  [[nodiscard]] std::vector<PlayerBet> roundBetsOf(network::RoomRound place);

  /** @brief Removes the bets @p userId holds on @p place. */
  void removeOf(network::RoomRound place, std::int64_t userId);

  /** @brief Writes @p bets as those @p userId holds on @p place, which holds
   * none of its bets: removeOf() comes first. */
  void insert(network::RoomRound place, std::int64_t userId,
              const std::vector<network::RouletteBet>& bets);

  /** @returns Every round before @p round that holds bets and was never
   * drawn, oldest first, whatever its room. */
  [[nodiscard]] std::vector<network::RoomRound> undrawnBefore(
      std::uint64_t round);

  /** @brief Removes the bets of every round drawn before @p before. */
  void removeDrawnBefore(std::chrono::sys_seconds before);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
