#include <gtest/gtest.h>
#include "BydoControlSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::BydoControlSystem;
using rtype::game::EntityType;
using rtype::game::Intent;

/**
 * Given a Bydo whose Intent wants nothing
 * When the Bydo control runs
 * Then the Intent of the Bydo wants to shoot
 */
TEST(BydoControlSystem, MakesABydoWantToShoot) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity bydo = entities.create();
  components.add(bydo, EntityType::Enemy);
  components.add(bydo, Intent{});
  BydoControlSystem control;

  control.update(components, SIMULATION_TICK_DURATION);

  const Intent* const intent = components.get<Intent>(bydo);
  ASSERT_NE(intent, nullptr);
  EXPECT_TRUE(intent->shooting);
}

/**
 * Given the ship of a player whose Intent wants nothing
 * When the Bydo control runs
 * Then the Intent of the ship is unchanged: only a Bydo is steered by it
 */
TEST(BydoControlSystem, LeavesTheIntentOfAShipAlone) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  const Entity ship = entities.create();
  components.add(ship, EntityType::Player);
  components.add(ship, Intent{});
  BydoControlSystem control;

  control.update(components, SIMULATION_TICK_DURATION);

  const Intent* const intent = components.get<Intent>(ship);
  ASSERT_NE(intent, nullptr);
  EXPECT_FALSE(intent->shooting);
}
