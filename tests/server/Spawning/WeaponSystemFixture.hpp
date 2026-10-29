#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world, a roster and a weapon system that works on them, the
 * datagrams it sends recorded. The Bydos are steered by their controller at
 * each tick; the ships by the Intent the test writes.
 */
class WeaponSystemFixture : public ::testing::Test {
 public:
  WeaponSystemFixture();

 protected:
  /** @brief Connects the player at @p index and gives it a ship where the ship
   * of its number starts. */
  ConnectedPlayer& connect(std::size_t index);

  /** @brief Connects the player at @p index, who is then told about each
   * projectile, without a ship. */
  void connectWithoutShip(std::size_t index);

  /** @brief Makes a Drifter appear at @p position, as the spawner does. */
  void bydoAt(game::Position position);

  /** @brief Runs @p count ticks in which only @p shooter holds shoot. */
  void runTicksShooting(ConnectedPlayer& shooter, std::size_t count);

  /** @brief Runs @p count ticks in which no player holds shoot. */
  void runTicksWithoutShooting(std::size_t count);

  /** @brief Runs @p count ticks, the players holding what they held. */
  void runTicks(std::size_t count);

  /** @returns How many missiles of players the world holds. */
  [[nodiscard]] std::size_t missilesInTheWorld();

  /** @returns The player numbers the missiles of the world belong to. */
  [[nodiscard]] std::vector<std::uint8_t> ownersOfTheMissiles();

  /** @returns The EntitySpawned that announces each missile of an enemy in the
   * world, as the players must be told about it. */
  [[nodiscard]] std::vector<network::EntitySpawned> enemyMissilesInTheWorld();

  /** @returns The EntitySpawned sent to the player at @p index. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> datagramsSentTo(
      std::size_t index) const;

  /** @returns Where the ship of @p player is; the origin, after a failure, when
   * it has none. */
  [[nodiscard]] game::Position shipOf(const ConnectedPlayer& player);

 private:
  /** @brief Makes the ship of @p player want to shoot, or not. */
  void setShooting(const ConnectedPlayer& player, bool shooting);

  /** @brief Makes the ship of every connected player stop wanting to shoot. */
  void releaseEveryone();

  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  WeaponSystem weapons_;
};

}  // namespace rtype::server::testing
