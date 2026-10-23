#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "EntityDestruction.hpp"
#include "EntitySpawned.hpp"
#include "GameInstanceFixtures.hpp"

namespace rtype::server::testing {

/**
 * @brief The movement fixture, whose players hold actions for the next tick,
 * and whose event bus delivers the events of a tick at its end when the test
 * asks, as the server does.
 */
class GameInstanceShootingFixture : public GameInstanceMovementTest {
 protected:
  /** @brief @p player sends its next input, holding @p actions, which the next
   * tick reads. */
  void holdForNextTick(std::size_t player, std::uint8_t actions);

  /** @brief Runs @p count ticks, the event bus delivering the events of each
   * one at its end. */
  void runTicksDispatching(std::size_t count);

  /** @returns The EntitySpawned of the missiles sent to @p player, oldest
   * first. */
  [[nodiscard]] std::vector<network::EntitySpawned> missilesSentTo(
      std::size_t player);

  /** @brief Records every EntityDestruction the event bus delivers. */
  void recordDestructions();

  /**
   * @returns Whether exactly one recorded EntityDestruction names @p networkId
   * and @p cause.
   */
  [[nodiscard]] bool wasDestroyed(std::uint32_t networkId,
                                  game::DestructionCause cause) const;

 private:
  std::vector<game::EntityDestruction> destructions_;
};

}  // namespace rtype::server::testing
