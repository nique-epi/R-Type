#include "SessionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
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
  return network::PlayerInput{
      .inputNumber = inputNumber, .actions = actions, .earlierActions = {}};
}

network::PlayerInput inputRepeating(std::uint32_t inputNumber,
                                    std::uint8_t actions,
                                    std::vector<std::uint8_t> earlierActions) {
  return network::PlayerInput{.inputNumber = inputNumber,
                              .actions = actions,
                              .earlierActions = std::move(earlierActions)};
}

ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index) {
  return *roster.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                     SESSION_TOKEN);
}

void addPlayers(PlayerRoster& roster, std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    static_cast<void>(addPlayer(roster, index));
  }
}

void ignoreInput(const ConnectedPlayer& /*player*/,
                 const network::PlayerInput& /*input*/) {}

std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
recorderOf(std::vector<ShownInput>& shown) {
  return [&shown](const ConnectedPlayer& player,
                  const network::PlayerInput& input) {
    shown.push_back(ShownInput{.input = input,
                               .numberKeptBefore = player.newestInputNumber});
  };
}

}  // namespace rtype::server::testing
