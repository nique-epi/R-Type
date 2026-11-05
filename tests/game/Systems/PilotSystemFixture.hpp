#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "ActionKey.hpp"
#include "BehaviourRegistry.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "IBehaviour.hpp"
#include "MovementSystem.hpp"
#include "PilotSystem.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief Bodies piloted by the built-in behaviours at the speed of a Bydo,
 * moved by a PilotSystem then a MovementSystem, as a game runs them.
 */
class PilotSystemFixture : public ::testing::Test {
 protected:
  PilotSystemFixture();

  /** @returns A body at @p start, at BODY_TOP_SPEED, piloted by the behaviour
   * named @p behaviour with the values @p settings, which must outlive the
   * body (a constant array of the tests). */
  engine::Entity addBody(std::string_view behaviour,
                         std::span<const std::int64_t> settings,
                         Position start);

  /** @returns An enemy piloted by @p behaviour, which must outlive it, with
   * the network id @p networkId and an idle CurrentAction. */
  engine::Entity addEnemy(const IBehaviour& behaviour, std::uint32_t networkId);

  /** @brief Adds the ship of a player at @p position, which no behaviour
   * pilots. */
  void addShip(Position position);

  /** @returns The EnemyActionStarted events published since the last call,
   * once the bus delivered them. */
  [[nodiscard]] std::vector<EnemyActionStarted> takeStartedActions();

  /** @returns The action @p enemy is in, nothing while it is idle or has no
   * CurrentAction. */
  [[nodiscard]] std::optional<ActionKey> actionOf(engine::Entity enemy);

  /** @brief Runs the pilots, then the movement, once over @p elapsed. */
  void run(engine::Duration elapsed);

  /** @returns Where @p body is; the origin when it has no Position. */
  [[nodiscard]] Position positionOf(engine::Entity body);

  engine::ComponentRegistry& components();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  std::vector<EnemyActionStarted> startedActions_;
  BehaviourRegistry behaviours_;
  PilotSystem pilots_;
  MovementSystem movement_;
};

}  // namespace rtype::game::testing
