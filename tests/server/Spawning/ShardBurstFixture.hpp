#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "BodyPlacement.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EventBus.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world holding the orb and the shard of the sheet tests, and the
 * weapon system that makes the orb burst; the datagrams it sends and the
 * removals it announces are recorded. Nothing moves the bodies.
 */
class ShardBurstFixture : public ::testing::Test {
 protected:
  ShardBurstFixture();

  /** @returns The network id of an orb made to appear at @p center. */
  std::uint32_t orbAt(game::Position center);

  /** @brief Connects a player, who is then told about each shard. */
  void connectPlayer();

  /** @brief Runs @p count ticks of the weapon system. */
  void runTicks(std::size_t count);

  /** @returns Where each shard of the world is, and the point its Pilot
   * steers from. */
  [[nodiscard]] std::vector<game::BodyPlacement> shards();

  /** @returns The EntityDestruction announced so far, once delivered. */
  [[nodiscard]] std::vector<game::EntityDestruction> removals();

  /** @returns The EntitySpawned sent to the player connected first. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSent() const;

 private:
  game::EntitySheets sheets_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  engine::EventBus events_;
  std::vector<game::EntityDestruction> removals_;
  WeaponSystem weapons_;
};

}  // namespace rtype::server::testing
