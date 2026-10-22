#pragma once

#include "EffectConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The chain of kills of the squadron: kills less than
 * KILL_CHAIN_WINDOW apart, by any pilot. Every BIG_CATCH_EVERY-th kill of a
 * chain is a big catch, which plays the effects one tier higher. Nothing is
 * shown of the chain itself: a counter on screen would promise points the
 * server does not give.
 */
class KillChain {
 public:
  /**
   * @brief Counts a kill: it extends the chain when it comes less than
   * KILL_CHAIN_WINDOW after the previous kill, else starts a new one.
   * @returns Whether it is a big catch.
   */
  [[nodiscard]] bool noteKill();

  /** @brief Moves the time since the last kill on. */
  void advance(engine::Duration elapsed);

 private:
  int length_{};
  engine::Duration sinceKill_{KILL_CHAIN_WINDOW};
};

}  // namespace rtype::client
