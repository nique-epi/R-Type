#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <span>
#include <string_view>
#include "BehaviourRegistry.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
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

  /** @brief Runs the pilots, then the movement, once over @p elapsed. */
  void run(engine::Duration elapsed);

  /** @returns Where @p body is; the origin when it has no Position. */
  [[nodiscard]] Position positionOf(engine::Entity body);

  engine::ComponentRegistry& components();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  BehaviourRegistry behaviours_;
  PilotSystem pilots_;
  MovementSystem movement_;
};

}  // namespace rtype::game::testing
