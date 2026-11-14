#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Direction.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/**
 * @brief A registry, a bus and the exit system that works on them, every
 * EntityDestruction it announces recorded once dispatched.
 */
class PlayfieldExitSystemFixture : public ::testing::Test {
 protected:
  PlayfieldExitSystemFixture();

  /** @returns A new networked entity of @p type at @p positionX, on the row
   * the tests use; a missile gets the box of the sheet of its side. */
  engine::Entity add(EntityType type, float positionX);

  /** @returns A new networked missile of @p type at @p positionX, on the row
   * the tests use, that collides with @p box. */
  engine::Entity addMissileWithBox(EntityType type, float positionX,
                                   CollisionBox box);

  /** @returns A new networked missile of an enemy at @p position, with the
   * box of its sheet, that a pilot steers towards @p heading. */
  engine::Entity addSteeredMissile(Position position, Direction heading);

  /** @returns A new networked obstacle at @p positionX, on the row the tests
   * use, that collides with @p box. */
  engine::Entity addObstacle(float positionX, CollisionBox box);

  /** @returns A new networked enemy at @p positionX, on the row the tests
   * use, that is a boss. */
  engine::Entity addBoss(float positionX);

  /** @brief Runs the system for one tick, then delivers what it published. */
  void updateAndDispatch();

  [[nodiscard]] const engine::EntityRegistry& entities() const;
  [[nodiscard]] const std::vector<EntityDestruction>& received() const;

 private:
  /** @returns A new networked entity of @p type at @p positionX, on the row
   * the tests use, with no box. */
  engine::Entity addWithoutBox(EntityType type, float positionX);

  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  PlayfieldExitSystem exit_{events_};
  std::vector<EntityDestruction> received_;
};

}  // namespace rtype::game::testing
