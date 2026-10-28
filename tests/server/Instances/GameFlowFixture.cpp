#include "GameFlowFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameFlow.hpp"
#include "GameFlowParts.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RtypeSystems.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "Spawning/WavePlanFixtures.hpp"

namespace rtype::server::testing {

GameFlowFixture::GameFlowFixture()
    : readyKeeper_(roster_, [] { return true; }),
      replication_(roster_, [](const network::OutgoingDatagram&) {}),
      wavePlan_(testWavePlan()),
      flow_(GameFlowParts{
          .events = events_,
          .systems = systems_,
          .world = world_,
          .roster = roster_,
          .replication = replication_,
          .send = [](const network::OutgoingDatagram&) {},
          .everyPlayerIsReady =
              [this] { return readyKeeper_.everyPlayerIsReady(); },
          .seed = SOME_SEED,
          .waves = wavePlan_}) {
  addRtypeSystems(systems_, events_);
}

ConnectedPlayer& GameFlowFixture::join(std::size_t index) {
  ConnectedPlayer* const player = roster_.add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  flow_.onPlayerJoined(*player);
  return *player;
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
