#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "ActionKey.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "HurtZone.hpp"
#include "HurtZoneSystem.hpp"
#include "ShipContact.hpp"

namespace rtype::game::testing {

/**
 * @brief A scene with a HurtZoneSystem and the ShipContacts it published and
 * the bus delivered.
 */
class HurtZoneSystemFixture : public ::testing::Test {
 public:
  HurtZoneSystemFixture();

 protected:
  /** @brief Adds an enemy at (ENEMY_X, ENEMY_Y) with @p zone. */
  void addEnemy(HurtZone zone);

  /** @brief Adds a ship centered at (@p centerX, ENEMY_Y). */
  void addShip(float centerX);

  /** @brief Puts the enemy in @p action. */
  void enterAction(ActionKey action);

  /** @brief Runs the system once and delivers its events.
   * @returns Every ShipContact delivered since the start. */
  const std::vector<ShipContact>& run();

 private:
  CollisionScene scene_;
  HurtZoneSystem system_;
  std::vector<ShipContact> contacts_;
  engine::Entity enemy_{};
};

}  // namespace rtype::game::testing
