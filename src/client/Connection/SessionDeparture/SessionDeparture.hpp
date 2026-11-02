#pragma once

#include <optional>
#include "Logger.hpp"
#include "SessionOutbox.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The copies of a Disconnection still due to a server the player left.
 * Leaving sends the first at once and keeps a copy of the outbox of the
 * session left, so the others go out from update() under its token and its
 * sequences, DISCONNECTION_COPY_INTERVAL apart, DISCONNECTION_COPY_COUNT in
 * all.
 */
class SessionDeparture {
 public:
  SessionDeparture();

  /** @brief Leaves the session @p session sends in: the first copy goes at
   * once, in place of any departure still going. */
  void leave(const SessionOutbox& session);

  /** @brief Sends the next copy once DISCONNECTION_COPY_INTERVAL passed since
   * the previous one. */
  void update(engine::Duration elapsed);

  /** @returns Whether copies are still due. */
  [[nodiscard]] bool isLeaving() const;

 private:
  void sendCopy();

  logging::Logger logger_;
  std::optional<SessionOutbox> leaving_;
  int copiesLeft_{0};
  engine::Duration sinceCopy_{};
};

}  // namespace rtype::client
