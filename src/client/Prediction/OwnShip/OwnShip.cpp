#include "OwnShip.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include "Position.hpp"
#include "WorldState.hpp"

namespace rtype::client {

std::optional<OwnShip> ownShipOf(const network::WorldState& state,
                                 std::uint8_t playerNumber) {
  const auto player = std::ranges::find(state.players, playerNumber,
                                        &network::PlayerState::playerNumber);
  if (player == state.players.end()) {
    return std::nullopt;
  }
  const auto ship = std::ranges::find(state.entities, player->shipEntityId,
                                      &network::EntityState::entityId);
  if (ship == state.entities.end()) {
    return std::nullopt;
  }
  return OwnShip{.shipEntityId = player->shipEntityId,
                 .position = game::Position{.x = ship->x, .y = ship->y},
                 .acknowledgedInput = player->acknowledgedInput};
}

}  // namespace rtype::client
