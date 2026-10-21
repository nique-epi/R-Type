#include "ShipControl.hpp"
#include <algorithm>
#include <chrono>
#include <numbers>
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns 1 when @p held goes right, -1 when it goes left, 0 when it holds
 * both or neither. */
int horizontalOf(ShipDirections held) {
  return static_cast<int>(held.right) - static_cast<int>(held.left);
}

/** @returns 1 when @p held goes down, -1 when it goes up, 0 when it holds both
 * or neither. */
int verticalOf(ShipDirections held) {
  return static_cast<int>(held.down) - static_cast<int>(held.up);
}

/**
 * @returns How fast a ship goes along each axis it moves on: SHIP_SPEED on one
 * axis, SHIP_SPEED divided by the square root of 2 on both, so a diagonal
 * covers SHIP_SPEED per second too.
 */
float speedPerAxis(bool diagonal) {
  return diagonal ? SHIP_SPEED / std::numbers::sqrt2_v<float> : SHIP_SPEED;
}

/** @returns @p ship moved the least needed for the whole ship to be inside the
 * playfield. */
Position keptInsidePlayfield(Position ship) {
  const float halfWidth = SHIP_WIDTH / 2.0F;
  const float halfHeight = SHIP_HEIGHT / 2.0F;
  return Position{
      .x = std::clamp(ship.x, halfWidth, PLAYFIELD_WIDTH - halfWidth),
      .y = std::clamp(ship.y, halfHeight, PLAYFIELD_HEIGHT - halfHeight)};
}

}  // namespace

Position shipPositionAfter(Position from, ShipDirections held,
                           engine::Duration elapsed) {
  const int horizontal = horizontalOf(held);
  const int vertical = verticalOf(held);
  const float distancePerAxis = speedPerAxis(horizontal != 0 && vertical != 0) *
                                std::chrono::duration<float>(elapsed).count();
  return keptInsidePlayfield(
      Position{.x = from.x + (distancePerAxis * static_cast<float>(horizontal)),
               .y = from.y + (distancePerAxis * static_cast<float>(vertical))});
}

}  // namespace rtype::game
