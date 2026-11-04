#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "SubscriptionHandle.hpp"
#include "World.hpp"

namespace rtype::game::testing {

/**
 * @brief A world holding one enemy, a bus, and the EnemyActionStarted events
 * the bus delivered.
 */
class EnemyActionsFixture : public ::testing::Test {
 public:
  EnemyActionsFixture();
  ~EnemyActionsFixture() override;

  EnemyActionsFixture(const EnemyActionsFixture&) = delete;
  EnemyActionsFixture& operator=(const EnemyActionsFixture&) = delete;
  EnemyActionsFixture(EnemyActionsFixture&&) = delete;
  EnemyActionsFixture& operator=(EnemyActionsFixture&&) = delete;

 protected:
  /** @returns The enemy the fixture spawned. */
  [[nodiscard]] engine::Entity enemy() const;

  /** @returns The network id of that enemy. */
  [[nodiscard]] std::uint32_t enemyNetworkId() const;

  /** @returns The registry of the world. */
  engine::ComponentRegistry& components();

  /** @returns The bus the publisher publishes on. */
  engine::EventBus& events();

  /** @brief Delivers the queued events and returns every event delivered
   * since the start, in order. */
  const std::vector<EnemyActionStarted>& delivered();

 private:
  World world_;
  engine::EventBus events_;
  engine::SubscriptionHandle subscription_;
  std::uint32_t enemyNetworkId_{0};
  engine::Entity enemy_{};
  std::vector<EnemyActionStarted> delivered_;
};

}  // namespace rtype::game::testing
