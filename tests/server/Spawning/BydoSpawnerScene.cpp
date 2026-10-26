#include "BydoSpawnerScene.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BydoSpawner.hpp"
#include "BydoSpawnerParts.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "MovementPattern.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "WavePlan.hpp"
#include "WavePlanFixtures.hpp"
#include "World.hpp"
#include "game/Waves/WaveTestConstants.hpp"

namespace rtype::server::testing {

void BydoSpawnerScene::connect(std::size_t index) {
  roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
              SESSION_TOKEN);
}

BydoSpawner BydoSpawnerScene::makeSpawner(std::uint32_t seed) {
  return makeSpawnerWith(seed, testWavePlan());
}

BydoSpawner BydoSpawnerScene::makeSpawnerWith(std::uint32_t seed,
                                              WavePlan waves) {
  return BydoSpawner{
      BydoSpawnerParts{.world = world_,
                       .roster = roster_,
                       .send =
                           [this](network::OutgoingDatagram datagram) {
                             sent_.push_back(std::move(datagram));
                           },
                       .seed = seed,
                       .waves = std::move(waves)}};
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

std::vector<float> BydoSpawnerScene::heightsOfTheFirstWave(std::uint32_t seed) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(seed);
  runTicks(spawner, game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE);
  return scene.heightsSentTo(FIRST_PLAYER);
}

void BydoSpawnerScene::clearTheSky() {
  world_.components().forEach<game::EntityType, game::EntityType>(
      [this](engine::Entity entity, const game::EntityType& type,
             const game::EntityType&) {
        if (type == game::EntityType::Enemy) {
          world_.components().destroy(entity);
        }
      });
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
