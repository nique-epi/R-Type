#pragma once

#include "ConnectionPhase.hpp"
#include "ConnectionStatus.hpp"

namespace rtype::client {

/**
 * @brief Notices the end of a session the server had accepted: a status that
 * turns Lost, the server having gone silent, or Closed, the server having
 * said it shuts down. ClientScreens asks it once per frame, so it answers
 * yes once per end, however long the status stays so.
 */
class SessionLossWatch {
 public:
  /** @returns Whether @p status ends a session: Lost or Closed, while the
   * status the previous call saw had another phase. */
  [[nodiscard]] bool endsTheSession(const ConnectionStatus& status);

 private:
  ConnectionPhase seen_ = ConnectionPhase::Idle;
};

}  // namespace rtype::client
