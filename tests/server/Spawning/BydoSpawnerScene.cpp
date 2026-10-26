#include "BydoSpawnerScene.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BydoSpawner.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "MovementPattern.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "World.hpp"

namespace rtype::server::testing {

void BydoSpawnerScene::connect(std::size_t index) {
  roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
              SESSION_TOKEN);
}

BydoSpawner BydoSpawnerScene::makeSpawner(std::uint32_t seed) {
  return BydoSpawner{world_, roster_,
                     [this](network::OutgoingDatagram datagram) {
                       sent_.push_back(std::move(datagram));
                     },
                     seed};
}

void BydoSpawnerScene::runTicks(BydoSpawner& spawner, std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    spawner.onTick();
  }
}

std::vector<float> BydoSpawnerScene::heightsSentTo(std::size_t index) const {
  std::vector<float> heights;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    heights.push_back(onlyMessageIn<network::EntitySpawned>(datagram).y);
  }
  return heights;
}

std::vector<network::EntitySpawned> BydoSpawnerScene::spawnsSent() const {
  std::vector<network::EntitySpawned> spawns;
  spawns.reserve(sent_.size());
  for (const network::OutgoingDatagram& datagram : sent_) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

std::vector<float> BydoSpawnerScene::heightsWithin(std::uint32_t seed,
                                                   std::size_t ticks) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(seed);
  runTicks(spawner, ticks);
  return scene.heightsSentTo(FIRST_PLAYER);
}

std::vector<game::PatternShape> BydoSpawnerScene::patternsOfTheBydos() {
  std::vector<game::PatternShape> shapes;
  world_.components().forEach<game::MovementPattern, game::MovementPattern>(
      [&shapes](engine::Entity, const game::MovementPattern& pattern,
                const game::MovementPattern&) {
        shapes.push_back(pattern.shape);
      });
  return shapes;
}

game::World& BydoSpawnerScene::world() { return world_; }

}  // namespace rtype::server::testing
