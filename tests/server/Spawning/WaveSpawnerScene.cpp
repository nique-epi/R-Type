#include "WaveSpawnerScene.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "Pilot.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "WavePlan.hpp"
#include "WavePlanFixtures.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerParts.hpp"
#include "World.hpp"
#include "game/Waves/WaveTestConstants.hpp"

namespace rtype::server::testing {

void WaveSpawnerScene::connect(std::size_t index) {
  roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
              SESSION_TOKEN);
}

WaveSpawner WaveSpawnerScene::makeSpawner(std::uint32_t seed) {
  return makeSpawnerWith(seed, testWavePlan());
}

WaveSpawner WaveSpawnerScene::makeSpawnerWith(std::uint32_t seed,
                                              WavePlan waves) {
  return WaveSpawner{
      WaveSpawnerParts{.world = world_,
                       .roster = roster_,
                       .send =
                           [this](network::OutgoingDatagram datagram) {
                             sent_.push_back(std::move(datagram));
                           },
                       .seed = seed,
                       .waves = std::move(waves),
                       .types = game::testing::shippedEntitySheets()}};
}

void WaveSpawnerScene::runTicks(WaveSpawner& spawner, std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    spawner.onTick();
  }
}

std::vector<float> WaveSpawnerScene::heightsSentTo(std::size_t index) const {
  std::vector<float> heights;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    heights.push_back(onlyMessageIn<network::EntitySpawned>(datagram).y);
  }
  return heights;
}

std::vector<network::EntitySpawned> WaveSpawnerScene::spawnsSent() const {
  std::vector<network::EntitySpawned> spawns;
  spawns.reserve(sent_.size());
  for (const network::OutgoingDatagram& datagram : sent_) {
    spawns.push_back(onlyMessageIn<network::EntitySpawned>(datagram));
  }
  return spawns;
}

std::vector<float> WaveSpawnerScene::heightsOfTheFirstWave(std::uint32_t seed) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner = scene.makeSpawner(seed);
  runTicks(spawner, game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE);
  return scene.heightsSentTo(FIRST_PLAYER);
}

void WaveSpawnerScene::clearTheSky() {
  world_.components().forEach<game::EntityType, game::EntityType>(
      [this](engine::Entity entity, const game::EntityType& type,
             const game::EntityType&) {
        if (type == game::EntityType::Enemy) {
          world_.components().destroy(entity);
        }
      });
}

std::vector<std::string_view> WaveSpawnerScene::behavioursOfTheBydos() {
  std::vector<std::string_view> names;
  world_.components().forEach<game::Pilot, game::Pilot>(
      [&names](engine::Entity, const game::Pilot& pilot, const game::Pilot&) {
        names.push_back(pilot.behaviour.get().name());
      });
  return names;
}

game::World& WaveSpawnerScene::world() { return world_; }

}  // namespace rtype::server::testing
