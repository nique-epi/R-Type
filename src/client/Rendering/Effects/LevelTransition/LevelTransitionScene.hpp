#pragma once

#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The transition of the level the game screen plays, as the LevelStatus
 * of the server tells it: the time it has left, which each frame counts down.
 * A status that tells none ends it. While it lasts, the player holds nothing,
 * as the server holds every ship.
 */
class LevelTransitionScene {
 public:
  /** @brief Plays a transition with @p millisecondsLeft, as a LevelStatus
   * tells it; 0 ends the one it played. */
  void show(std::uint16_t millisecondsLeft);

  /** @brief Lets @p elapsed of the transition pass. */
  void advance(engine::Duration elapsed);

  /** @returns The time left of the transition, zero when it plays none. */
  [[nodiscard]] engine::Duration left() const;

  /** @returns Whether a transition plays, so the player holds nothing. */
  [[nodiscard]] bool holdsTheShips() const;

 private:
  engine::Duration left_{};
};

}  // namespace rtype::client
