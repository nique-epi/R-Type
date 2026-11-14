#include <gtest/gtest.h>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Position.hpp"
#include "ShipFlight.hpp"
#include "ShipFlightSystem.hpp"
#include "ShipFlightSystemTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::ShipFlight;
using rtype::client::ShipFlightSystem;
using rtype::client::testing::PREDICTED_ROLL_SHARE;
using rtype::client::testing::START_OF_A_DRAWN_ROLL;
using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::Position;

/**
 * Given the ship of another pilot that a world state just started saying
 * rolls
 * When a frame runs
 * Then its roll is drawn from its start
 */
TEST(ShipFlightSystem, ARollAWorldStateStartsIsDrawnFromItsStart) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity ship = entities.create();
  components.add(ship, Position{});
  components.add(ship, ShipFlight{.rolling = true});

  ShipFlightSystem{}.update(components, SIMULATION_TICK_DURATION);

  const ShipFlight* const flight = components.get<ShipFlight>(ship);
  ASSERT_NE(flight, nullptr);
  EXPECT_EQ(flight->rollShare.value_or(-1.0F), START_OF_A_DRAWN_ROLL);
}

/**
 * Given the own ship of the player, whose prediction wrote how far through its
 * roll it is, while a world state says it rolls
 * When a frame runs
 * Then its roll stays where the prediction put it: the system leaves it
 */
TEST(ShipFlightSystem, APredictedRollKeepsTheShareOfThePrediction) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity ship = entities.create();
  components.add(ship, Position{});
  components.add(ship, ShipFlight{.rolling = true,
                                  .rollShare = PREDICTED_ROLL_SHARE,
                                  .predicted = true});

  ShipFlightSystem{}.update(components, SIMULATION_TICK_DURATION);

  const ShipFlight* const flight = components.get<ShipFlight>(ship);
  ASSERT_NE(flight, nullptr);
  EXPECT_EQ(flight->rollShare.value_or(-1.0F), PREDICTED_ROLL_SHARE);
}
