#include "PlayerControllerFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include "ConnectedPlayer.hpp"
#include "PlayerControllerTestConstants.hpp"
#include "PlayerInput.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

ConnectedPlayer& PlayerControllerFixture::addPlayer(
    std::size_t index, std::uint64_t sessionToken) {
  return *roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                      sessionToken);
}

ConnectedPlayer& PlayerControllerFixture::addPlayerWithShip(
    std::size_t index, std::uint64_t sessionToken) {
  ConnectedPlayer& player = addPlayer(index, sessionToken);
  player.shipEntityId = world_.spawnPlayer(CONTROLLED_SHIP);
  return player;
}

network::PlayerInput PlayerControllerFixture::inputHolding(
    std::uint8_t actions, std::uint32_t number) {
  return network::PlayerInput{.inputNumber = number, .actions = actions};
}

game::World& PlayerControllerFixture::world() { return world_; }

PlayerRoster& PlayerControllerFixture::roster() { return roster_; }

}  // namespace rtype::server::testing
