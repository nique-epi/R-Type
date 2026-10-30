#include "GameFlowFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameFlow.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "Lives.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RtypeSystems.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "Spawning/WavePlanFixtures.hpp"

namespace rtype::server::testing {

GameFlowFixture::GameFlowFixture()
    : readyKeeper_(roster_, [] { return true; }),
      replication_(
          roster_, [](const network::OutgoingDatagram&) {},
          DEFAULT_WORLD_STATES_PER_SECOND),
      context_{
          .events = events_,
          .timers = timers_,
          .systems = systems_,
          .roster = roster_,
          .hooks = hooks_,
          .settings = GameSettings{.seed = SOME_SEED, .waves = testWavePlan()},
          .send = [](const network::OutgoingDatagram&) {},
          .everyPlayerIsReady =
              [this] { return readyKeeper_.everyPlayerIsReady(); }},
      flow_(context_, world_, replication_) {
  flow_.begin();
  addRtypeSystems(systems_, events_);
}

ConnectedPlayer& GameFlowFixture::join(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  replication_.onPlayerJoined(*player);
  return *player;
}

void GameFlowFixture::takeEveryLifeOf(ConnectedPlayer& player) {
  auto* const lives =
      world_.current().shipComponentOf<game::Lives>(player.shipEntityId);
  ASSERT_NE(lives, nullptr);
  lives->count = 0;
}

void GameFlowFixture::startGameWith(
    std::initializer_list<std::size_t> indexes) {
  for (const std::size_t index : indexes) {
    join(index).ready = true;
  }
  tick();
  ASSERT_EQ(flow_.state(), GameState::InProgress);
}

void GameFlowFixture::startGameWithOnePlayer() {
  startGameWith({FIRST_PLAYER});
}

void GameFlowFixture::tick() {
  flow_.onTick();
  events_.dispatch();
}

void GameFlowFixture::runTicks(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    tick();
  }
}

GameFlow& GameFlowFixture::flow() { return flow_; }

PlayerRoster& GameFlowFixture::roster() { return roster_; }

}  // namespace rtype::server::testing
