#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "BydoGunner.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world, a roster and a gunner that works on them, the datagrams it
 * sends recorded.
 */
class BydoGunnerFixture : public ::testing::Test {
 public:
  BydoGunnerFixture();

 protected:
  /** @brief Connects the player at @p index, who is then told about each
   * missile. */
  void connect(std::size_t index);

  /** @brief Makes a Drifter appear at @p position, as the spawner does. */
  void bydoAt(game::Position position);

  /** @brief Runs @p count ticks of the gunner. */
  void runTicks(std::size_t count);

  /** @returns The EntitySpawned that announces each missile of an enemy in the
   * world, as the players must be told about it. */
  [[nodiscard]] std::vector<network::EntitySpawned> enemyMissilesInTheWorld();

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> datagramsSentTo(
      std::size_t index) const;

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  BydoGunner gunner_;
};

}  // namespace rtype::server::testing
