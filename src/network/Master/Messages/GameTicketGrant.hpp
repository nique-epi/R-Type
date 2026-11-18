#pragma once

#include <chrono>
#include <string>

namespace rtype::network {

/**
 * @brief What the master answers when it gave a game ticket to the account of
 * a session: the ticket, which it never shows again, and the second it
 * expires.
 */
struct GameTicketGrant {
  /** @brief GAME_TICKET_PREFIX then lowercase hexadecimal, GAME_TICKET_SIZE
   * long. */
  std::string ticket;
  std::chrono::sys_seconds expiresAt;

  bool operator==(const GameTicketGrant&) const = default;
};

}  // namespace rtype::network
