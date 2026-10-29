#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "BydoControlSystem.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world, a roster and a weapon system that works on them, the
 * datagrams it sends recorded. The Bydos are steered by their control at each
 * tick; the ships, which ShipWeaponFixture adds, by the Intent a test writes.
 */
class WeaponSystemFixture : public ::testing::Test {
 public:
  WeaponSystemFixture();

 protected:
  /** @brief Connects the player at @p index, who is then told about each
   * projectile, without a ship. */
  void connectWithoutShip(std::size_t index);

  /** @brief Makes a Drifter appear at @p position, as the spawner does. */
  void bydoAt(game::Position position);

  /** @brief Runs @p count ticks, the players wanting what they wanted. */
  void runTicks(std::size_t count);

  /** @returns The EntitySpawned that announces each missile of an enemy in the
   * world, as the players must be told about it. */
  [[nodiscard]] std::vector<network::EntitySpawned> enemyMissilesInTheWorld();

  /** @returns The EntitySpawned sent to the player at @p index. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> datagramsSentTo(
      std::size_t index) const;

  [[nodiscard]] game::World& world();
  [[nodiscard]] PlayerRoster& roster();

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  WeaponSystem weapons_;
  game::BydoControlSystem bydoControl_;
};

}  // namespace rtype::server::testing
