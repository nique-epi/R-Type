#include "GameInstanceLivesFixture.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "BydoKind.hpp"
#include "ConnectedPlayer.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "LifeKeeperTestConstants.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "Replication/GameEndAnnouncerTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

void GameInstanceLivesFixture::hitTheShipOf(std::size_t player) {
  world().spawnBydo(shipPositionOf(player), game::BydoKind::Drifter);
  runTicksDispatching(1);
}

std::uint32_t GameInstanceLivesFixture::shootTheShipOf(std::size_t player) {
  const std::uint32_t missile =
      world().spawnEnemyMissile(shipPositionOf(player));
  runTicksDispatching(1);
  return missile;
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

void GameInstanceLivesFixture::takeTheLastLifeOf(std::size_t player) {
  ConnectedPlayer* const target = game().roster().find(playerEndpoint(player));
  ASSERT_NE(target, nullptr);
  target->lives = ONE_LIFE;
  hitTheShipOf(player);
}

void GameInstanceLivesFixture::loseEveryLife() {
  takeTheLastLifeOf(FIRST_PLAYER);
  takeTheLastLifeOf(SECOND_PLAYER);
  runTicksDispatching(1);
}

void GameInstanceLivesFixture::giveTheFinalScores() {
  ConnectedPlayer* const first =
      game().roster().find(playerEndpoint(FIRST_PLAYER));
  ConnectedPlayer* const second =
      game().roster().find(playerEndpoint(SECOND_PLAYER));
  ASSERT_NE(first, nullptr);
  ASSERT_NE(second, nullptr);
  first->score = FIRST_PLAYER_FINAL_SCORE;
  second->score = SECOND_PLAYER_FINAL_SCORE;
}

}  // namespace rtype::server::testing
