#pragma once

#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief How far a level has scrolled: it moves on at LEVEL_SCROLL_SPEED
 * while nothing pauses it, from 0 when the level starts.
 *
 * The time it scrolled is kept whole, so the position never drifts with the
 * number of ticks: position() is that time at LEVEL_SCROLL_SPEED.
 */
class LevelScroll {
 public:
  /** @brief Scrolls for @p elapsed, unless paused. A negative time moves
   * nothing. */
  void advance(engine::Duration elapsed);

  /** @brief Stops the scroll where it is until resume(). */
  void pause();

  /** @brief Lets the scroll move on again. */
  void resume();

  /** @brief Puts the scroll back where it was after @p travelled of scroll,
   * paused or not as it is. */
  void returnTo(engine::Duration travelled);

  /** @returns The time the level scrolled, its pauses left out. */
  [[nodiscard]] engine::Duration travelled() const;

  /** @returns How far the level scrolled, in logical units. */
  [[nodiscard]] float position() const;

  /** @returns The speed of the scroll now, in logical units per second: 0
   * while paused. */
  [[nodiscard]] float speed() const;

 private:
  engine::Duration travelled_{engine::Duration::zero()};
  bool paused_{false};
};

}  // namespace rtype::game
