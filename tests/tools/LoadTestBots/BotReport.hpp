#pragma once

#include <cstddef>
#include "ConnectionPhase.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What a bot saw of its run. */
struct BotReport {
  /** @brief Where its connection stands when the report is taken. */
  client::ConnectionPhase phase{client::ConnectionPhase::Idle};
  /** @brief The GameEnded it received: one per game it saw end. */
  std::size_t gamesEnded{0};
  /** @brief The world states it received, each tick counted once. */
  std::size_t worldStates{0};
  /** @brief The time it spent in a game, from the first world state of each
   * game to its GameEnded. */
  engine::Duration timeInGame{};
};

}  // namespace rtype::tools
