#include "ShipControl.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <optional>
#include "CollisionConstants.hpp"
#include "ManeuverKind.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"
#include "ShipMotion.hpp"
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
 * play area: the playfield above the status band. */
Position keptInsidePlayArea(Position ship) {
  const float halfWidth = SHIP_WIDTH * engine::HALF;
  const float halfHeight = SHIP_HEIGHT * engine::HALF;
  return Position{
      .x = std::clamp(ship.x, halfWidth, PLAYFIELD_WIDTH - halfWidth),
      .y = std::clamp(ship.y, halfHeight, PLAY_AREA_HEIGHT - halfHeight)};
}

/** @brief The maneuver a second press asks for: what it is, and how far it
 * carries the ship. */
struct ManeuverOrder {
  ManeuverKind kind{ManeuverKind::None};
  Position distance{};
};

/** @brief One step of the drift of an axis: the speed it aims at, and how
 * long the step lasts, in seconds. */
struct DriftStep {
  float target{};
  float seconds{};
};

/** @returns @p speed drifted over @p step towards its target, as
 * ShipControlConstants.hpp tells the drift: the rise constants while the axis
 * is held, a target other than 0, the fall ones once it is not. */
float driftedSpeed(float speed, DriftStep step) {
  const float target = step.target;
  const float seconds = step.seconds;
  const bool held = target != 0.0F;
  const float rate = held ? DRIFT_RISE_RATE : DRIFT_FALL_RATE;
  const float floor = held ? DRIFT_RISE_FLOOR : DRIFT_FALL_FLOOR;
  const float gap = target - speed;
  const float closed = std::max(
      std::abs(gap) * (1.0F - std::exp(-rate * seconds)), floor * seconds);
  if (std::abs(gap) <= closed) {
    return target;
  }
  return speed + std::copysign(closed, gap);
}

/** @returns 1 - (1 - @p share)^3: fast first, then slow. The easeOutCubic of
 * the client's Rendering/Easing, which rtype_game may not link. */
float easedOut(float share) {
  const float left = 1.0F - share;
  return 1.0F - (left * left * left);
}

/** @returns The share of its distance a maneuver of @p duration has carried
 * the ship after @p time, eased out, 1 from its end on. */
float carriedShare(engine::Duration time, engine::Duration duration) {
  const float share = std::chrono::duration<float>(time) /
                      std::chrono::duration<float>(duration);
  return easedOut(std::min(share, 1.0F));
}

/** @returns How long a maneuver of @p kind lasts. */
engine::Duration durationOf(ManeuverKind kind) {
  return kind == ManeuverKind::Roll ? ROLL_DURATION : DASH_DURATION;
}

/**
 * @returns How far the maneuver of @p motion carries the ship over the next
 * @p elapsed; the maneuver runs on, and ends once it reaches its duration.
 * Nothing without a maneuver.
 */
Position carriedBy(ShipMotion& motion, engine::Duration elapsed) {
  if (motion.maneuver == ManeuverKind::None) {
    return Position{};
  }
  const engine::Duration duration = durationOf(motion.maneuver);
  const float before = carriedShare(motion.maneuverTime, duration);
  motion.maneuverTime += elapsed;
  const float share = carriedShare(motion.maneuverTime, duration) - before;
  if (motion.maneuverTime >= duration) {
    motion.maneuver = ManeuverKind::None;
  }
  return Position{.x = motion.maneuverDistance.x * share,
                  .y = motion.maneuverDistance.y * share};
}

/** @returns @p ship kept inside the play area, its velocity along an axis
 * lost where an edge stopped it. */
MovingShip stoppedAtTheEdges(MovingShip ship) {
  const Position kept = keptInsidePlayArea(ship.position);
  if (kept.x != ship.position.x) {
    ship.motion.velocityX = 0.0F;
  }
  if (kept.y != ship.position.y) {
    ship.motion.velocityY = 0.0F;
  }
  ship.position = kept;
  return ship;
}

/**
 * @brief Records the press of @p direction in @p change into @p lastPress.
 * @returns Whether it was a second press within DOUBLE_TAP_INPUTS of the
 * last one; the record then goes back to none.
 */
bool notePress(HeldChange change, bool ShipDirections::* direction,
               std::uint32_t& lastPress) {
  if (change.before.*direction || !(change.now.*direction)) {
    return false;
  }
  const bool second =
      lastPress != 0 && change.inputNumber - lastPress <= DOUBLE_TAP_INPUTS;
  lastPress = second ? 0 : change.inputNumber;
  return second;
}

/** @returns The directions @p change presses a second time, every press
 * recorded in @p motion. */
ShipDirections pressedTwice(ShipMotion& motion, HeldChange change) {
  return ShipDirections{
      .up = notePress(change, &ShipDirections::up, motion.lastUpPress),
      .down = notePress(change, &ShipDirections::down, motion.lastDownPress),
      .left = notePress(change, &ShipDirections::left, motion.lastLeftPress),
      .right =
          notePress(change, &ShipDirections::right, motion.lastRightPress)};
}

/** @returns The maneuver directions pressed twice ask for: down or up a
 * roll, right or left a dash; nothing without one. */
std::optional<ManeuverOrder> orderOf(ShipDirections twice) {
  if (twice.down || twice.up) {
    return ManeuverOrder{
        .kind = ManeuverKind::Roll,
        .distance = Position{.x = 0.0F,
                             .y = twice.down ? ROLL_SIDESTEP : -ROLL_SIDESTEP}};
  }
  if (twice.right || twice.left) {
    return ManeuverOrder{
        .kind = ManeuverKind::Dash,
        .distance = Position{
            .x = twice.right ? DASH_DISTANCE : -BACKSTEP_DISTANCE, .y = 0.0F}};
  }
  return std::nullopt;
}

/** @returns Whether @p motion may start a maneuver of @p kind: none runs, and
 * the wait of that kind, rollWait for a roll and dashWait for a dash or a
 * backstep, is over. */
bool mayStart(const ShipMotion& motion, ManeuverKind kind) {
  const engine::Duration wait =
      kind == ManeuverKind::Roll ? motion.rollWait : motion.dashWait;
  return motion.maneuver == ManeuverKind::None &&
         wait <= engine::Duration::zero();
}

/** @returns @p motion running @p order from its start; a roll also makes the
 * next roll wait its duration and ROLL_COOLDOWN, a dash or a backstep the
 * next of them its duration and DASH_COOLDOWN. */
ShipMotion started(ShipMotion motion, ManeuverOrder order) {
  motion.maneuver = order.kind;
  motion.maneuverDistance = order.distance;
  motion.maneuverTime = engine::Duration::zero();
  if (order.kind == ManeuverKind::Roll) {
    motion.rollWait = ROLL_DURATION + ROLL_COOLDOWN;
  }
  if (order.kind == ManeuverKind::Dash) {
    motion.dashWait = DASH_DURATION + DASH_COOLDOWN;
  }
  return motion;
}

}  // namespace

MovingShip shipAfter(MovingShip from, ShipDirections held,
                     engine::Duration elapsed) {
  const int horizontal = horizontalOf(held);
  const int vertical = verticalOf(held);
  const float top = speedPerAxis(horizontal != 0 && vertical != 0);
  const float seconds = std::chrono::duration<float>(elapsed).count();
  ShipMotion motion = from.motion;
  motion.velocityX =
      driftedSpeed(motion.velocityX,
                   DriftStep{.target = top * static_cast<float>(horizontal),
                             .seconds = seconds});
  motion.velocityY = driftedSpeed(
      motion.velocityY, DriftStep{.target = top * static_cast<float>(vertical),
                                  .seconds = seconds});
  motion.rollWait =
      std::max(motion.rollWait - elapsed, engine::Duration::zero());
  motion.dashWait =
      std::max(motion.dashWait - elapsed, engine::Duration::zero());
  const Position carried = carriedBy(motion, elapsed);
  const Position wanted{
      .x = from.position.x + (motion.velocityX * seconds) + carried.x,
      .y = from.position.y + (motion.velocityY * seconds) + carried.y};
  return stoppedAtTheEdges(MovingShip{.position = wanted, .motion = motion});
}

ShipMotion motionAfterInput(ShipMotion motion, HeldChange change) {
  const std::optional<ManeuverOrder> order =
      orderOf(pressedTwice(motion, change));
  if (!order.has_value() || !mayStart(motion, order->kind)) {
    return motion;
  }
  return started(motion, *order);
}

}  // namespace rtype::game
