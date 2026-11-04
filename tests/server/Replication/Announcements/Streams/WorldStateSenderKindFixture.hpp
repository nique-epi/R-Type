#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EnemyKind.hpp"
#include "EntityRegistry.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "WorldState.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server::testing {

/**
 * @brief A sender to one connected player, and a registry in which enemies of
 * a given kind stand.
 */
class WorldStateSenderKindFixture : public ::testing::Test {
 public:
  WorldStateSenderKindFixture();

 protected:
  /** @brief Adds an enemy of @p kind under @p networkId. */
  void addEnemy(game::EnemyKind kind, std::uint32_t networkId);

  /** @returns The entities the first state sent to the player lists. */
  [[nodiscard]] std::vector<network::EntityState> listedEntities();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  WorldStateSender sender_;
};

}  // namespace rtype::server::testing
