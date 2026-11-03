#pragma once

#include <optional>
#include "ConnectionPhase.hpp"

namespace rtype::tools {

/** @brief What a bot sees of its session when its squad decides whether it
 * asks to be ready. */
struct BotStanding {
  client::ConnectionPhase phase{client::ConnectionPhase::Idle};
  /** @brief Whether the server announced the start of its game. */
  bool gameStarted{false};
  /** @brief The ready state it asked for in this waiting room, if any. */
  std::optional<bool> requestedReady;
};

}  // namespace rtype::tools
