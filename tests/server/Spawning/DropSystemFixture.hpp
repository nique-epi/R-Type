#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DropSystem.hpp"
#include "EntitySpawned.hpp"
#include "EventBus.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A drop system on its own world, with the shipped sheets, that
 * records what it sends to its players.
 */
class DropSystemFixture : public ::testing::Test {
 protected:
  DropSystemFixture();

  /** @brief Connects the player at @p index, without a ship. */
  void connect(std::size_t index);

  /** @brief Publishes that an enemy dropped the red crystal at @p where, and
   * delivers it, as DamageRules and the server do. */
  void dropRedAt(game::Position where);

  /** @brief Runs one tick of the system. */
  void tick();

  /** @returns The EntitySpawned sent to the player at @p index, oldest
   * first. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @returns How many crystals the world holds. */
  [[nodiscard]] std::size_t crystalsInTheWorld();

  /** @returns The number of the shipped sheet of the red crystal. */
  [[nodiscard]] static std::uint8_t redCrystalNumber();

 private:
  engine::EventBus events_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  DropSystem drops_;
};

}  // namespace rtype::server::testing
