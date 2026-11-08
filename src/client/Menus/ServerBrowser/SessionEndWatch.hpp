#pragma once

#include <optional>
#include "ConnectionPhase.hpp"
#include "IServerConnection.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Tells the server map when to ask for the games again after the
 * player came back from a server: once GAMES_RECHECK_DELAY passed since the
 * connection went from an attempt or a session back to no attempt, as BACK
 * from the connection screen or from the waiting room leaves it.
 *
 * The master learns that a player left from the heartbeat of the server, sent
 * at most once per network::MINIMUM_NOTICE_GAP, so a list asked at once could
 * still count the player who just left.
 */
class SessionEndWatch {
 public:
  /** @param connection The connection the player joins servers with, which
   * must outlive the watch. */
  explicit SessionEndWatch(const IServerConnection& connection);

  /** @returns Whether the games are to be asked for again now: true once per
   * attempt or session that ended, GAMES_RECHECK_DELAY after its end. */
  [[nodiscard]] bool recheckDue(engine::Duration elapsed);

 private:
  const IServerConnection* connection_;
  ConnectionPhase seen_;
  /** @brief The time since the last end not answered yet; nothing when no
   * end waits. */
  std::optional<engine::Duration> sinceEnd_;
};

}  // namespace rtype::client
