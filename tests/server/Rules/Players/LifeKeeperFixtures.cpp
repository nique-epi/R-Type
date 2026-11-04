#include "LifeKeeperFixtures.hpp"
#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "NetworkIdentity.hpp"
#include "SurvivesContact.hpp"
#include "World.hpp"

namespace rtype::server::testing {

namespace {

std::optional<engine::Entity> entityOf(game::World& world,
                                       std::uint32_t networkId) {
  std::optional<engine::Entity> found;
  world.components().forEach<game::NetworkIdentity, game::NetworkIdentity>(
      [&found, networkId](engine::Entity entity,
                          const game::NetworkIdentity& identity,
                          const game::NetworkIdentity&) {
        if (identity.networkId == networkId) {
          found = entity;
        }
      });
  return found;
}

}  // namespace

void markAsSurvivor(game::World& world, std::uint32_t networkId) {
  if (const std::optional<engine::Entity> entity = entityOf(world, networkId)) {
    world.components().add(*entity, game::SurvivesContact{});
  }
}

bool isInTheWorld(game::World& world, std::uint32_t networkId) {
  return entityOf(world, networkId).has_value();
}

}  // namespace rtype::server::testing
