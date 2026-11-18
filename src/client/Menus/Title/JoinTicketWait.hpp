#pragma once

#include <cstdint>
#include <optional>
#include "IAccountSession.hpp"
#include "ITicketLink.hpp"
#include "PendingJoin.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Holds the attempt of a signed-in player until the master gave it a
 * game ticket, TICKET_WAIT at most, so the server reports its game with the
 * ticket and the master pays its account. A player signed out, a master that
 * refuses or does not answer in time: the attempt goes as a guest. The ticket
 * stays in memory, in the request of the attempt only.
 */
class JoinTicketWait {
 public:
  /** @param account The account the attempts are made under; @p tickets
   * asks the master a ticket. Both must outlive the wait. */
  JoinTicketWait(const IAccountSession& account, ITicketLink& tickets);

  /**
   * @brief Replaces any attempt held with @p join.
   * @returns @p join at once, as a guest, when the player is signed out;
   * nothing when it is held, a ticket asked for it.
   */
  [[nodiscard]] std::optional<PendingJoin> begin(const PendingJoin& join);

  /** @brief Drops the attempt held, if any. */
  void drop();

  /** @returns Whether an attempt waits for its ticket. */
  [[nodiscard]] bool waiting() const;

  /**
   * @returns The attempt held, once @p elapsed more passed: with its ticket
   * once the master answered with one, as a guest once it refused or once
   * TICKET_WAIT passed; nothing while it waits, or when none is held. An
   * attempt is given once.
   */
  [[nodiscard]] std::optional<PendingJoin> advance(engine::Duration elapsed);

 private:
  const IAccountSession* account_;
  ITicketLink* tickets_;
  std::optional<PendingJoin> held_;
  std::uint64_t ask_{};
  engine::Duration waited_{};
};

}  // namespace rtype::client
