#include "WaveSpawnerCarrierFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>
#include "EntitySpawned.hpp"
#include "EntityTypesTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "WavePlan.hpp"
#include "WavePlanFixtures.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerScene.hpp"

namespace rtype::server::testing {

WavePlan wavePlanWithACarrier() {
  WavePlan plan = testWavePlan();
  plan.carrier = std::string{game::testing::CRYSTAL_CARRIER_SHEET};
  return plan;
}

std::vector<network::EntitySpawned> carriersIn(
    const std::vector<network::EntitySpawned>& spawns) {
  const std::uint8_t carrier = game::testing::shippedEntitySheets()
                                   .named(game::testing::CRYSTAL_CARRIER_SHEET)
                                   .number;
  std::vector<network::EntitySpawned> found;
  std::ranges::copy_if(spawns, std::back_inserter(found),
                       [carrier](const network::EntitySpawned& spawned) {
                         return spawned.entityType == carrier;
                       });
  return found;
}

void playClearingTheSky(WaveSpawnerScene& scene, WaveSpawner& spawner,
                        std::size_t ticks) {
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    WaveSpawnerScene::runTicks(spawner, 1);
    scene.clearTheSky();
  }
}

}  // namespace rtype::server::testing
