#include "OwnShipPredictor.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ActionSet.hpp"
#include "IPlayerInputLink.hpp"
#include "OwnShip.hpp"
#include "PlayerAction.hpp"
#include "Position.hpp"
#include "ShipDirections.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

game::ShipDirections directionsOf(ActionSet held) {
  return game::ShipDirections{.up = held.has(PlayerAction::Up),
                              .down = held.has(PlayerAction::Down),
                              .left = held.has(PlayerAction::Left),
                              .right = held.has(PlayerAction::Right)};
}

}  // namespace

OwnShipPredictor::OwnShipPredictor(std::uint8_t playerNumber)
    : playerNumber_(playerNumber) {}

void OwnShipPredictor::advance(engine::Duration elapsed, ActionSet held,
                               IPlayerInputLink& link) {
  uncounted_ += elapsed;
  const std::size_t counted = engine::wholeTicksIn(uncounted_);
  const std::size_t ticks =
      std::min(counted, engine::MAXIMUM_TICKS_PER_ADVANCE);
  uncounted_ = counted > ticks
                   ? engine::Duration::zero()
                   : uncounted_ - engine::SIMULATION_TICK_DURATION *
                                      static_cast<engine::Duration::rep>(ticks);
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    if (const std::optional<std::uint32_t> number =
            link.sendPlayerInput(held)) {
      prediction_.predict(directionsOf(held), *number);
    }
  }
}

void OwnShipPredictor::apply(const network::WorldState& state) {
  const std::optional<OwnShip> own = ownShipOf(state, playerNumber_);
  if (!own.has_value()) {
    return;
  }
  if (shipEntityId_ != own->shipEntityId) {
    shipEntityId_ = own->shipEntityId;
    prediction_.restart(own->position);
    return;
  }
  prediction_.reconcile(own->position, own->acknowledgedInput);
}

void OwnShipPredictor::stop() {
  if (!prediction_.isStarted() && uncounted_ == engine::Duration::zero()) {
    return;
  }
  prediction_ = ShipPrediction{};
  shipEntityId_.reset();
  uncounted_ = engine::Duration::zero();
}

std::optional<std::uint32_t> OwnShipPredictor::shipEntityId() const {
  return shipEntityId_;
}

game::Position OwnShipPredictor::position() const {
  return prediction_.position();
}

}  // namespace rtype::client
