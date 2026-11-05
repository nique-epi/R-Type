#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "PlayfieldExitSystem.hpp"

namespace rtype::game::testing {

/**
 * @brief A registry, a bus and the exit system that works on them, every
 * EntityDestruction it announces recorded once dispatched.
 */
class PlayfieldExitSystemFixture : public ::testing::Test {
 protected:
  PlayfieldExitSystemFixture();

  /** @returns A new networked entity of @p type at @p positionX, on the row
   * the tests use. */
  engine::Entity add(EntityType type, float positionX);

  /** @brief Runs the system for one tick, then delivers what it published. */
  void updateAndDispatch();

  [[nodiscard]] const engine::EntityRegistry& entities() const;
  [[nodiscard]] const std::vector<EntityDestruction>& received() const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  PlayfieldExitSystem exit_{events_};
  std::vector<EntityDestruction> received_;
};

}  // namespace rtype::game::testing
