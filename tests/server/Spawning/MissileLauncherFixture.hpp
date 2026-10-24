#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "MissileLauncher.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world, a roster and a launcher that works on them, the datagrams it
 * sends recorded.
 */
class MissileLauncherFixture : public ::testing::Test {
 public:
  MissileLauncherFixture();

 protected:
  /** @brief Connects the player at @p index and gives it a ship where the ship
   * of its number starts. */
  ConnectedPlayer& connect(std::size_t index);

  /** @brief Runs @p count ticks in which @p shooter holds shoot. */
  void runTicksShooting(ConnectedPlayer& shooter, std::size_t count);

  /** @brief Runs @p count ticks in which no player holds shoot. */
  void runTicksWithoutShooting(std::size_t count);

  /** @returns How many missiles of players the world holds. */
  [[nodiscard]] std::size_t missilesInTheWorld();

  /** @returns The player numbers the missiles of the world belong to. */
  [[nodiscard]] std::vector<std::uint8_t> ownersOfTheMissiles();

  /** @returns The EntitySpawned sent to the player at @p index. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @returns Where the ship of @p player is; the origin, after a failure, when
   * it has none. */
  [[nodiscard]] game::Position shipOf(const ConnectedPlayer& player);

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  MissileLauncher launcher_;
};

}  // namespace rtype::server::testing
