#pragma once

#include "Position.hpp"

namespace rtype::client {

/**
 * @brief The gap between the ship shown and the ship predicted, which shrinks
 * by a fixed share each tick so a correction glides instead of jumping.
 *
 * The predicted ship is never changed: the prediction keeps replaying from
 * the position of the server, and only what is drawn follows the gap. After a
 * correction the gap is the old position shown minus the new position
 * predicted, so the ship does not move on the frame of the correction and
 * then decays toward the prediction. A second correction while the gap is not
 * closed starts from the position really shown, so nothing jumps between two
 * corrections.
 */
class ShipCorrection {
 public:
  /** @brief Keeps the gap between @p shown and @p predicted so the ship
   * glides to @p predicted; a gap beyond SNAP_DISTANCE is dropped and the ship
   * is shown where it is predicted. */
  void absorb(game::Position shown, game::Position predicted);

  /** @brief Shrinks the gap by one tick. */
  void decay();

  /** @brief Forgets the gap: the ship is shown where it is predicted. */
  void clear();

  /** @returns @p predicted moved by the gap that remains. */
  [[nodiscard]] game::Position shownFor(game::Position predicted) const;

 private:
  game::Position gap_{};
};

}  // namespace rtype::client
