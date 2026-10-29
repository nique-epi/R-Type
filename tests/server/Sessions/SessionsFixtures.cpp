#include "SessionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "MessageOrigin.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

network::MessageOrigin originOf(std::size_t index, std::uint16_t sequence) {
  return network::MessageOrigin{
      .sender = playerEndpoint(index),
      .header = network::PacketHeader{.sequence = sequence}};
}

network::PlayerInput inputNumbered(std::uint32_t inputNumber,
                                   std::uint8_t actions) {
  return network::PlayerInput{.inputNumber = inputNumber, .actions = actions};
}

ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index) {
  return *roster.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                     SESSION_TOKEN);
}

void ignoreInput(const ConnectedPlayer& /*player*/,
                 const network::PlayerInput& /*input*/) {}

std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
recorderOf(std::vector<ShownInput>& shown) {
  return [&shown](const ConnectedPlayer& player,
                  const network::PlayerInput& input) {
    shown.push_back(
        ShownInput{.input = input, .keptBefore = player.newestInput});
  };
}

}  // namespace rtype::server::testing
