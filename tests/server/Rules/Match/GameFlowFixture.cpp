#include "GameFlowFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "GameEnded.hpp"
#include "GameFlow.hpp"
#include "GameSettings.hpp"
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "Lives.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "RtypeSystems.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "Spawning/WavePlanFixtures.hpp"
#include "WavePlan.hpp"

namespace rtype::server::testing {

GameFlowFixture::GameFlowFixture() : GameFlowFixture(testWavePlan()) {}

GameFlowFixture::GameFlowFixture(WavePlan waves)
    : readyKeeper_(roster_, [] { return true; }),
      context_{.events = events_,
               .systems = systems_,
               .roster = roster_,
               .hooks = hooks_,
               .settings =
                   GameSettings{.seed = SOME_SEED,
                                .waves = std::move(waves),
                                .types = game::testing::shippedEntitySheets()},
               .send =
                   [this](network::OutgoingDatagram datagram) {
                     sent_.push_back(std::move(datagram));
                   },
               .everyPlayerIsReady =
                   [this] { return readyKeeper_.everyPlayerIsReady(); }},
      replication_(context_),
      flow_(context_, world_, replication_) {
  static_cast<void>(events_.subscribe<GameStateChanged>(
      [this](const GameStateChanged& change) { changes_.push_back(change); }));
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
  ASSERT_EQ(flow_.state(), GameState::Starting);
  for (const std::size_t index : indexes) {
    roster_.find(playerEndpoint(index))->loaded = true;
  }
  tick();
  ASSERT_EQ(flow_.state(), GameState::InProgress);
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

void GameFlowFixture::clearTheBydos() {
  world_.current().components().forEach<game::EntityType, game::EntityType>(
      [this](engine::Entity entity, const game::EntityType& type,
             const game::EntityType&) {
        if (type == game::EntityType::Enemy) {
          world_.current().components().destroy(entity);
        }
      });
}

std::vector<network::GameEnded> GameFlowFixture::endsSentTo(
    std::size_t index) const {
  std::vector<network::GameEnded> ends;
  for (const network::OutgoingDatagram& datagram :
       testing::sentTo(sent_, playerEndpoint(index))) {
    if (onlyMessageTypeOf(datagram) == network::MessageType::GameEnded) {
      ends.push_back(onlyMessageIn<network::GameEnded>(datagram));
    }
  }
  return ends;
}

GameFlow& GameFlowFixture::flow() { return flow_; }

PlayerRoster& GameFlowFixture::roster() { return roster_; }

const std::vector<GameStateChanged>& GameFlowFixture::changes() const {
  return changes_;
}

GameFlowOneWaveLevelFixture::GameFlowOneWaveLevelFixture()
    : GameFlowFixture(oneWaveLevelWavePlan()) {}

}  // namespace rtype::server::testing
