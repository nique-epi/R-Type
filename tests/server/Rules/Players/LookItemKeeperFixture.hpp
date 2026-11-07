#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "LookItemKeeper.hpp"
#include "LookItemWon.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A keeper of the look items of one world, with real ships for its
 * players, that records what it sends and the destructions the bus delivers.
 */
class LookItemKeeperFixture : public ::testing::Test {
 protected:
  LookItemKeeperFixture();

  /** @brief Connects the player number @p index + 1 and gives it a ship. */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Puts a look item of the crown in the world.
   * @returns Its network id. */
  std::uint32_t spawnACrown();

  /** @brief Publishes the touch of the ship of @p player on the item
   * @p itemId, as CollisionRules does; the bus delivers it at deliver(). */
  void publishTouch(const ConnectedPlayer& player, std::uint32_t itemId);

  /** @brief Delivers what the bus holds, as the server does at the end of a
   * tick. */
  void deliver();

  /** @brief Runs one tick of the keeper. */
  void tick();

  /** @returns Whether the world still holds the entity of @p networkId. */
  [[nodiscard]] bool worldHolds(std::uint32_t networkId);

  /** @returns The LookItemWon sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::LookItemWon> wonBy(
      std::size_t index) const;

  /** @returns How many delivered EntityDestruction name @p networkId as
   * Removed. */
  [[nodiscard]] std::size_t removalsOf(std::uint32_t networkId) const;

 private:
  engine::EventBus events_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_;
  std::unique_ptr<LookItemKeeper> keeper_;
  std::vector<game::EntityDestruction> destructions_;
};

}  // namespace rtype::server::testing
