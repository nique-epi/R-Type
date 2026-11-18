#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "GameTicketState.hpp"
#include "IssuedGameTicket.hpp"

namespace rtype::master {

/** @brief The table `game_tickets`, the game tickets the master gave: the
 * only code that writes its SQL. It works on a connection the caller holds,
 * inside the caller's transaction. */
class GameTicketRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit GameTicketRepository(SQLite::Database& connection);

  void insert(const IssuedGameTicket& ticket);

  /** @returns How many tickets @p userId was given after @p since. */
  [[nodiscard]] std::int64_t issuedSince(std::int64_t userId,
                                         std::chrono::sys_seconds since);

  /** @brief Forgets the tickets expired before @p now. */
  void forgetExpired(std::chrono::sys_seconds now);

  /** @returns The ticket of @p fingerprint, nothing when @p userId was given
   * none of that fingerprint. */
  [[nodiscard]] std::optional<network::GameTicketState> stateOf(
      std::int64_t userId, const std::string& fingerprint);

  /** @brief Marks the ticket of @p fingerprint used at @p now, when it was
   * never used and has not expired.
   * @returns The account it was given to; nothing when no ticket has that
   * fingerprint, or it was used, or it expired. */
  [[nodiscard]] std::optional<std::int64_t> redeem(
      const std::string& fingerprint, std::chrono::sys_seconds now);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
