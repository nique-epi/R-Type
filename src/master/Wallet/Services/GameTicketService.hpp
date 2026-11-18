#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "GameTicketGrant.hpp"
#include "GameTicketState.hpp"
#include "MasterDatabase.hpp"

namespace rtype::master {

/**
 * @brief The rules of the game tickets: the master gives one to the account
 * of a session, keeps its fingerprint only, and a result of a game pays the
 * account through it once, before it expires (see WalletService::creditGame()).
 */
class GameTicketService {
 public:
  /** @param database It must outlive the service. */
  explicit GameTicketService(MasterDatabase& database);

  /**
   * @brief Gives @p userId a new ticket at @p now, which expires
   * GAME_TICKET_LIFETIME later, and forgets the tickets expired before
   * @p now.
   * @returns The ticket and its expiry; nothing, giving none, when the account
   * was given MAXIMUM_GAME_TICKETS_PER_WINDOW in the last GAME_TICKET_WINDOW.
   */
  [[nodiscard]] std::optional<network::GameTicketGrant> issue(
      std::int64_t userId, std::chrono::sys_seconds now);

  /** @returns The ticket of @p fingerprint, nothing when @p userId was given
   * none of that fingerprint, or it was forgotten. */
  [[nodiscard]] std::optional<network::GameTicketState> stateOf(
      std::int64_t userId, const std::string& fingerprint);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
