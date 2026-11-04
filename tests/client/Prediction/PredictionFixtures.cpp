#include "PredictionFixtures.hpp"
#include <cmath>
#include <cstdint>
#include "Position.hpp"
#include "PredictionTestConstants.hpp"
#include "ShipControl.hpp"
#include "ShipDirections.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

game::MovingShip shipAfterCommands(game::ShipDirections held,
                                   std::uint32_t count) {
  game::MovingShip ship{.position = START, .motion = {}};
  game::ShipDirections before{};
  for (std::uint32_t number = FIRST_COMMAND; number <= count; ++number) {
    ship.motion = game::motionAfterInput(
        ship.motion,
        game::HeldChange{.before = before, .now = held, .inputNumber = number});
    ship = game::shipAfter(ship, held, engine::SIMULATION_TICK_DURATION);
    before = held;
  }
  return ship;
}

float distanceBetween(game::Position first, game::Position second) {
  return std::hypot(first.x - second.x, first.y - second.y);
}

game::Position shiftedRight(game::Position ship, float offsetX) {
  return game::Position{.x = ship.x + offsetX, .y = ship.y};
}

network::WorldState stateWithShip(std::uint8_t playerNumber,
                                  std::uint32_t shipId, game::Position position,
                                  std::uint32_t acknowledgedInput) {
  return network::WorldState{
      .tick = 1,
      .players = {network::PlayerState{.playerNumber = playerNumber,
                                       .shipEntityId = shipId,
                                       .acknowledgedInput = acknowledgedInput}},
      .entities = {network::EntityState{
          .entityId = shipId,
          .entityType = network::WireEntityType::PlayerShip,
          .x = position.x,
          .y = position.y}}};
}

network::WorldState stateWithoutEntities(std::uint8_t playerNumber,
                                         std::uint32_t shipId) {
  return network::WorldState{
      .tick = 1,
      .players = {network::PlayerState{.playerNumber = playerNumber,
                                       .shipEntityId = shipId}},
      .entities = {}};
}

}  // namespace rtype::client::testing
