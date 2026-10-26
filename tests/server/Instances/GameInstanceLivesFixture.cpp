#include "GameInstanceLivesFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include "BydoKind.hpp"
#include "ConnectedPlayer.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

void GameInstanceLivesFixture::hitTheShipOf(std::size_t player) {
  game().world().spawnBydo(shipPositionOf(player), game::BydoKind::Drifter);
  runTicksDispatching(1);
}

void GameInstanceLivesFixture::startTheNextGame() {
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    sendReady({.player = player,
               .ready = true,
               .sequence = SECOND_SEQUENCE,
               .sessionToken = PLAYER_SESSION_TOKENS.at(player)});
  }
  runTicksDispatching(1);
}

ConnectedPlayer GameInstanceLivesFixture::playerAt(std::size_t index) {
  const ConnectedPlayer* const player =
      game().roster().find(playerEndpoint(index));
  EXPECT_NE(player, nullptr);
  return player == nullptr ? ConnectedPlayer{} : *player;
}

network::PlayerState GameInstanceLivesFixture::newestStateOf(
    std::size_t index) {
  const std::vector<network::OutgoingDatagram> states =
      ofType(network::MessageType::WorldState, sentTo(index));
  const std::vector<network::PlayerState> players =
      states.empty()
          ? std::vector<network::PlayerState>{}
          : onlyMessageIn<network::WorldState>(states.back()).players;
  const auto listed = std::ranges::find(players, playerAt(index).playerNumber,
                                        &network::PlayerState::playerNumber);
  EXPECT_NE(listed, players.end());
  return listed == players.end() ? network::PlayerState{} : *listed;
}

}  // namespace rtype::server::testing
