#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "LagCompensatedHitSystem.hpp"
#include "MissileHit.hpp"
#include "Position.hpp"
#include "ShipContact.hpp"

namespace rtype::game::testing {

/**
 * @brief A world with one LagCompensatedHitSystem and the ships it tests, the
 * MissileHit and ShipContact events it published recorded after each tick.
 */
class LagCompensatedHitSystemShipsFixture : public ::testing::Test {
 protected:
  LagCompensatedHitSystemShipsFixture();

  /** @returns A ship of a player at @p position whose player saw the enemies
   * @p ticksBehind ticks behind the present. */
  engine::Entity shipAt(Position position, std::size_t ticksBehind);

  /** @returns A missile of an enemy at @p position. */
  engine::Entity enemyMissileAt(Position position);

  /** @returns An enemy at @p position whose HurtZone has one box, hurting all
   * the time, and no body to run into. */
  engine::Entity hurtZoneAt(Position position);

  /** @brief Puts @p entity at @p position. */
  void move(engine::Entity entity, Position position);

  /** @brief Runs @p count ticks of the system, each followed by its events. */
  void runTicks(std::size_t count);

  /** @returns The hits published so far, oldest first. */
  [[nodiscard]] const std::vector<MissileHit>& hits() const;

  /** @returns The contacts published so far, oldest first. */
  [[nodiscard]] const std::vector<ShipContact>& contacts() const;

  [[nodiscard]] engine::ComponentRegistry& components();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  LagCompensatedHitSystem system_{events_};
  std::vector<MissileHit> hits_;
  std::vector<ShipContact> contacts_;
};

}  // namespace rtype::game::testing
