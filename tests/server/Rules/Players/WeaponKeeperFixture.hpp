#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Gun.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "WeaponKeeper.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A keeper of the crystals of one world, with real ships for its
 * players, that records the destructions the bus delivers.
 */
class WeaponKeeperFixture : public ::testing::Test {
 protected:
  WeaponKeeperFixture();

  /** @brief Connects the player number @p index + 1 and gives it a ship. */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Puts a crystal of the shipped sheet @p sheet in the world.
   * @returns Its network id. */
  std::uint32_t spawnCrystal(std::string_view sheet);

  /** @brief Publishes the touch of the ship of @p player on the crystal
   * @p crystalId, as CollisionRules does, and delivers it. */
  void touch(const ConnectedPlayer& player, std::uint32_t crystalId);

  /** @brief Runs one tick of the keeper, then delivers what it published, as
   * the server does at the end of a tick. */
  void tick();

  /** @returns Whether the world still holds the entity of @p networkId. */
  [[nodiscard]] bool worldHolds(std::uint32_t networkId);

  /** @returns The Gun of the ship of @p player; a default one, after a
   * failure, when it has none. */
  [[nodiscard]] game::Gun gunOf(const ConnectedPlayer& player);

  /** @returns How many delivered EntityDestruction name @p networkId as
   * Removed. */
  [[nodiscard]] std::size_t removalsOf(std::uint32_t networkId) const;

 private:
  engine::EventBus events_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_;
  std::unique_ptr<WeaponKeeper> keeper_;
  std::vector<game::EntityDestruction> destructions_;
};

}  // namespace rtype::server::testing
