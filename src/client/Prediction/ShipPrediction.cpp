#include "ShipPrediction.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
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

/** @returns @p ship one tick later, @p held pressed now after @p before. */
game::MovingShip oneTickLater(game::MovingShip ship, game::HeldChange change,
                              game::ShipDirections held) {
  ship.motion = game::motionAfterInput(ship.motion, change);
  return game::shipAfter(ship, held, engine::SIMULATION_TICK_DURATION);
}

}  // namespace

void ShipPrediction::restart(game::Position start) {
  started_ = true;
  current_ = game::MovingShip{.position = start, .motion = {}};
  lastHeld_ = {};
  pending_.clear();
}

void ShipPrediction::predict(game::ShipDirections held,
                             std::uint32_t inputNumber) {
  if (!started_) {
    return;
  }
  current_ = oneTickLater(
      current_,
      game::HeldChange{
          .before = lastHeld_, .now = held, .inputNumber = inputNumber},
      held);
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

bool ShipPrediction::isStarted() const { return started_; }

game::Position ShipPrediction::position() const { return current_.position; }

void ShipPrediction::dropBefore(std::uint32_t inputNumber) {
  while (!pending_.empty() && pending_.front().inputNumber < inputNumber) {
    pending_.pop_front();
  }
}

void ShipPrediction::replayAfterFront(game::MovingShip restart) {
  pending_.front().after = restart;
  game::MovingShip replayed = restart;
  game::ShipDirections before = pending_.front().held;
  for (std::size_t index = 1; index < pending_.size(); ++index) {
    PendingCommand& command = pending_[index];
    replayed =
        oneTickLater(replayed,
                     game::HeldChange{.before = before,
                                      .now = command.held,
                                      .inputNumber = command.inputNumber},
                     command.held);
    command.after = replayed;
    before = command.held;
  }
  current_ = replayed;
}

}  // namespace rtype::client
