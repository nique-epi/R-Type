#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "LifeKeeper.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A keeper of the lives of one world, which gives ships back through a
 * real ShipSpawnAnnouncer, and records the events the bus delivers.
 */
class LifeKeeperFixture : public ::testing::Test {
 protected:
  LifeKeeperFixture();

  /** @brief Connects the player number @p index + 1 and gives it a ship. */
  ConnectedPlayer& join(std::size_t index);

  /**
   * @brief Puts a Drifter on the ship of @p player.
   * @returns The network id of the Drifter.
   */
  std::uint32_t bydoOn(const ConnectedPlayer& player);

  /** @brief Publishes the contact of the ship @p shipId and the enemy
   * @p enemyId, as CollisionRules does; the bus delivers it at deliver(). */
  void publishContact(std::uint32_t shipId, std::uint32_t enemyId);

  /** @brief Delivers what the bus holds, as the server does at the end of a
   * tick. */
  void deliver();

  /**
   * @brief Puts a Drifter on the ship of @p player and delivers their contact.
   * @returns The network id of the Drifter.
   */
  std::uint32_t touchABydo(const ConnectedPlayer& player);

  /** @returns The lives @p player has left, its ship in play included. */
  [[nodiscard]] std::uint8_t livesOf(const ConnectedPlayer& player);

  /** @brief Gives the ship of @p player @p lives lives. */
  void setLives(const ConnectedPlayer& player, std::uint8_t lives);

  /** @returns Whether the ship of @p player is invulnerable. */
  [[nodiscard]] bool isInvulnerable(const ConnectedPlayer& player);

  /** @brief Runs @p count ticks of the keeper. */
  void runTicks(std::size_t count);

  /** @brief Destroys the keeper; the bus no longer reaches it. */
  void destroyTheKeeper();

  /** @returns How many delivered EntityDestruction name @p networkId as
   * Killed. */
  [[nodiscard]] std::size_t explosionsOf(std::uint32_t networkId) const;

  [[nodiscard]] const std::vector<game::PlayerHit>& hits() const;
  [[nodiscard]] const std::vector<game::PlayerElimination>& eliminations()
      const;

 private:
  engine::EventBus events_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_;
  std::unique_ptr<LifeKeeper> keeper_;
  std::vector<game::EntityDestruction> destructions_;
  std::vector<game::PlayerHit> hits_;
  std::vector<game::PlayerElimination> eliminations_;
};

}  // namespace rtype::server::testing
