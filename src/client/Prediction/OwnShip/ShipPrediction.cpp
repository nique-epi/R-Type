#include "ShipPrediction.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include "Bounds.hpp"
#include "ObstacleBlocking.hpp"
#include "Position.hpp"
#include "PredictionConstants.hpp"
#include "ShipControl.hpp"
#include "ShipDirections.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

float distanceBetween(game::Position first, game::Position second) {
  return std::hypot(first.x - second.x, first.y - second.y);
}

/** @returns @p ship one tick later, after the input @p change: the presses it
 * records first, then the tick holding the directions it holds now, stopped
 * against @p obstacles and kept inside the play area, as the server moves
 * it. */
game::MovingShip oneTickLater(game::MovingShip ship, game::HeldChange change,
                              std::span<const engine::Bounds> obstacles) {
  ship.motion = game::motionAfterInput(ship.motion, change);
  return game::blockedByObstacles(
      ship, game::shipAfter(ship, change.now, engine::SIMULATION_TICK_DURATION),
      obstacles);
}

}  // namespace

void ShipPrediction::restart(game::Position start, std::uint32_t acknowledged) {
  started_ = true;
  current_ = game::MovingShip{.position = start, .motion = {}};
  dropBefore(acknowledged + 1);
  if (pending_.empty()) {
    lastHeld_ = {};
    return;
  }
  replayFrom(0, current_);
  lastHeld_ = pending_.back().held;
}

void ShipPrediction::predict(game::ShipDirections held,
                             std::uint32_t inputNumber) {
  if (!started_) {
    if (pending_.size() == KEPT_COMMANDS_BEFORE_A_SHIP) {
      pending_.pop_front();
    }
    pending_.push_back(
        PendingCommand{.inputNumber = inputNumber, .held = held, .after = {}});
    return;
  }
  current_ = oneTickLater(
      current_,
      game::HeldChange{
          .before = lastHeld_, .now = held, .inputNumber = inputNumber},
      obstacles_);
  lastHeld_ = held;
  pending_.push_back(PendingCommand{
      .inputNumber = inputNumber, .held = held, .after = current_});
}

void ShipPrediction::reconcile(game::Position server,
                               std::uint32_t acknowledged) {
  if (!started_) {
    return;
  }
  dropBefore(acknowledged);
  if (pending_.empty() || pending_.front().inputNumber != acknowledged) {
    return;
  }
  const game::MovingShip& confirmed = pending_.front().after;
  if (distanceBetween(confirmed.position, server) <= RECONCILIATION_TOLERANCE) {
    return;
  }
  replayAfterFront(
      game::MovingShip{.position = server, .motion = confirmed.motion});
}

void ShipPrediction::seeObstacles(std::span<const engine::Bounds> obstacles) {
  obstacles_ = obstacles;
}

bool ShipPrediction::isStarted() const { return started_; }

game::Position ShipPrediction::position() const { return current_.position; }

void ShipPrediction::dropBefore(std::uint32_t inputNumber) {
  while (!pending_.empty() && pending_.front().inputNumber < inputNumber) {
    pending_.pop_front();
  }
}

void ShipPrediction::replayAfterFront(game::MovingShip restart) {
  pending_.front().after = restart;
  replayFrom(1, restart);
}

void ShipPrediction::replayFrom(std::size_t first, game::MovingShip from) {
  game::MovingShip replayed = from;
  game::ShipDirections before =
      first > 0 ? pending_[first - 1].held : pending_[first].held;
  for (std::size_t index = first; index < pending_.size(); ++index) {
    PendingCommand& command = pending_[index];
    replayed =
        oneTickLater(replayed,
                     game::HeldChange{.before = before,
                                      .now = command.held,
                                      .inputNumber = command.inputNumber},
                     obstacles_);
    command.after = replayed;
    before = command.held;
  }
  current_ = replayed;
}

}  // namespace rtype::client
