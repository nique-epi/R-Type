#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "LagCompensatedHitSystem.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/**
 * @brief A world with one LagCompensatedHitSystem, the MissileHit events it
 * published recorded after each tick.
 */
class LagCompensatedHitSystemFixture : public ::testing::Test {
 protected:
  LagCompensatedHitSystemFixture();

  /** @returns An enemy at @p position, with the box of an enemy. */
  engine::Entity enemyAt(Position position);

  /** @returns A missile of a player at @p position whose player saw the
   * enemies @p ticksBehind ticks behind the present. */
  engine::Entity missileAt(Position position, std::size_t ticksBehind);

  /** @brief Puts @p entity at @p position. */
  void move(engine::Entity entity, Position position);

  /** @brief Runs the system for one tick, then delivers its events. */
  void runTick();

  /** @brief Gives the system the registries of a new world from the next
   * tick, built before the old ones are freed, as a new game builds its
   * world. */
  void startANewWorld();

  /** @brief Runs @p count ticks. */
  void runTicks(std::size_t count);

  /** @returns The hits published so far, oldest first. */
  [[nodiscard]] const std::vector<MissileHit>& hits() const;

  [[nodiscard]] engine::ComponentRegistry& components();

 private:
  std::unique_ptr<engine::EntityRegistry> entities_ =
      std::make_unique<engine::EntityRegistry>();
  std::unique_ptr<engine::ComponentRegistry> components_ =
      std::make_unique<engine::ComponentRegistry>(*entities_);
  engine::EventBus events_;
  LagCompensatedHitSystem system_{events_};
  std::vector<MissileHit> hits_;
};

}  // namespace rtype::game::testing
