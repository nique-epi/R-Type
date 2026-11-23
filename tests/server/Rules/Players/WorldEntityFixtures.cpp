#include "WorldEntityFixtures.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

engine::Entity entityWithNetworkId(game::World& world,
                                   std::uint32_t networkId) {
  std::optional<engine::Entity> found;
  world.components().forEach<game::NetworkIdentity, game::Position>(
      [&found, networkId](engine::Entity entity,
                          const game::NetworkIdentity& identity,
                          const game::Position& /*position*/) {
        if (identity.networkId == networkId) {
          found = entity;
        }
      });
  EXPECT_TRUE(found.has_value());
  return found.value_or(engine::Entity{});
}

}  // namespace rtype::server::testing
