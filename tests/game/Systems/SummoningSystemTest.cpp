#include <gtest/gtest.h>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Summoning.hpp"
#include "SummoningSystem.hpp"
#include "SummoningSystemTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::Summoning;
using rtype::game::SummoningSystem;
using rtype::game::testing::LAST_TICK_OF_SUMMONING;
using rtype::game::testing::THREE_TICKS_OF_SUMMONING;
using rtype::game::testing::TWO_TICKS_OF_SUMMONING;

/**
 * Given a body summoned for three ticks
 * When the summoning system runs once
 * Then two ticks of its summoning are left
 */
TEST(SummoningSystem, CountsOneTickOffASummoning) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity body = entities.create();
  components.add(body, Summoning{.ticksLeft = THREE_TICKS_OF_SUMMONING});
  SummoningSystem system;

  system.update(components, SIMULATION_TICK_DURATION);

  ASSERT_NE(components.get<Summoning>(body), nullptr);
  EXPECT_EQ(components.get<Summoning>(body)->ticksLeft, TWO_TICKS_OF_SUMMONING);
}

/**
 * Given a body on the last tick of its summoning
 * When the summoning system runs once
 * Then the body is summoned no more: it takes part
 */
TEST(SummoningSystem, LetsABodyTakePartOnceItsSummoningIsOver) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity body = entities.create();
  components.add(body, Summoning{.ticksLeft = LAST_TICK_OF_SUMMONING});
  SummoningSystem system;

  system.update(components, SIMULATION_TICK_DURATION);

  EXPECT_FALSE(components.has<Summoning>(body));
}

/**
 * Given a body summoned for two ticks
 * When the summoning system runs once
 * Then the body is still summoned
 */
TEST(SummoningSystem, KeepsABodySummonedBeforeItsLastTick) {
  EntityRegistry entities;
  ComponentRegistry components{entities};
  const Entity body = entities.create();
  components.add(body, Summoning{.ticksLeft = TWO_TICKS_OF_SUMMONING});
  SummoningSystem system;

  system.update(components, SIMULATION_TICK_DURATION);

  EXPECT_TRUE(components.has<Summoning>(body));
}
