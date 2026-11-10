#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BehaviourScripts.hpp"
#include "EngineException.hpp"
#include "EntityDestruction.hpp"
#include "EntityOrders.hpp"
#include "EntityOrdersFixture.hpp"
#include "EntityOrdersTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "Position.hpp"

using rtype::engine::ScriptException;
using rtype::game::BehaviourScripts;
using rtype::game::DestructionCause;
using rtype::game::Position;
using rtype::network::EntitySpawned;
using rtype::server::EntityOrders;
using rtype::server::testing::DRIFTER_SHEET;
using rtype::server::testing::EntityOrdersFixture;
using rtype::server::testing::PLAYERS_TOLD;
using rtype::server::testing::SCRIPTED_REMOVALS_IN_A_TICK;
using rtype::server::testing::SCRIPTED_SPAWNS_IN_A_TICK;
using rtype::server::testing::SHIP_SHEET;
using rtype::server::testing::UNKNOWN_SHEET;
using rtype::server::testing::WHERE_A_SCRIPT_PUTS_IT;
using rtype::server::testing::WHERE_THE_SHIP_FLIES;

/**
 * Given a script that asks for a drifter during a tick
 * When the tick has not ended yet
 * Then no body has the id the script was given
 */
TEST_F(EntityOrdersFixture, CreatesNothingBeforeTheEndOfTheTick) {
  const std::uint32_t networkId =
      spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);

  EXPECT_FALSE(positionOf(networkId).has_value());
}

/**
 * Given a script that asked for a drifter
 * When the tick ends
 * Then a body with the id the script was given stands where it asked
 */
TEST_F(EntityOrdersFixture, CreatesTheBodyWithTheIdTheScriptWasGiven) {
  const std::uint32_t networkId =
      spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);

  endTick();

  const std::optional<Position> position = positionOf(networkId);
  ASSERT_TRUE(position.has_value());
  const Position found = position.value_or(Position{});
  EXPECT_EQ(found.x, WHERE_A_SCRIPT_PUTS_IT.x);
  EXPECT_EQ(found.y, WHERE_A_SCRIPT_PUTS_IT.y);
}

/**
 * Given two connected players and a script that asked for a drifter
 * When the tick ends
 * Then each player is told about the body, under the id the script was given
 */
TEST_F(EntityOrdersFixture, TellsEveryPlayerAboutTheBody) {
  connectPlayers(PLAYERS_TOLD);
  const std::uint32_t networkId =
      spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);

  endTick();

  for (std::size_t player = 0; player < PLAYERS_TOLD; ++player) {
    const std::vector<EntitySpawned> told = spawnsSentTo(player);
    ASSERT_EQ(told.size(), 1U);
    EXPECT_EQ(told.front().entityId, networkId);
  }
}

/**
 * Given a script that names a sheet no game has
 * When it asks for a body of it
 * Then the request is refused
 */
TEST_F(EntityOrdersFixture, RefusesASheetThatDoesNotExist) {
  EXPECT_THROW(
      static_cast<void>(spawnByScript(UNKNOWN_SHEET, WHERE_A_SCRIPT_PUTS_IT)),
      ScriptException);
}

/**
 * Given a script that names the sheet of the ships
 * When it asks for a body of it
 * Then the request is refused
 */
TEST_F(EntityOrdersFixture, RefusesToCreateAShip) {
  EXPECT_THROW(
      static_cast<void>(spawnByScript(SHIP_SHEET, WHERE_A_SCRIPT_PUTS_IT)),
      ScriptException);
}

/**
 * Given a script that asked for every body a tick allows
 * When it asks for one more in the same tick
 * Then that one is refused
 */
TEST_F(EntityOrdersFixture, RefusesABodyPastTheLimitOfATick) {
  for (std::size_t body = 0; body < SCRIPTED_SPAWNS_IN_A_TICK; ++body) {
    static_cast<void>(spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT));
  }

  EXPECT_THROW(
      static_cast<void>(spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT)),
      ScriptException);
}

/**
 * Given a drifter of the game and a script that asks for its removal
 * When the tick ends
 * Then the drifter is gone
 */
TEST_F(EntityOrdersFixture, RemovesTheBodyAScriptNames) {
  const std::uint32_t networkId =
      placeDirectly(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);
  removeByScript(networkId);

  endTick();

  EXPECT_FALSE(positionOf(networkId).has_value());
}

/**
 * Given a drifter of the game and a script that asks for its removal
 * When the tick ends
 * Then its removal is published for the players
 */
TEST_F(EntityOrdersFixture, PublishesTheRemoval) {
  const std::uint32_t networkId =
      placeDirectly(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);
  removeByScript(networkId);

  endTick();

  ASSERT_EQ(removals().size(), 1U);
  EXPECT_EQ(removals().front().networkId, networkId);
  EXPECT_EQ(removals().front().cause, DestructionCause::Removed);
}

/**
 * Given the ship of a player and a script that asks for its removal
 * When the tick ends
 * Then the ship is still there
 */
TEST_F(EntityOrdersFixture, NeverRemovesAShip) {
  const std::uint32_t networkId =
      placeDirectly(SHIP_SHEET, WHERE_THE_SHIP_FLIES);
  removeByScript(networkId);

  endTick();

  EXPECT_TRUE(positionOf(networkId).has_value());
}

/**
 * Given a script that creates a drifter and removes it in the same tick
 * When the tick ends
 * Then the drifter is gone
 */
TEST_F(EntityOrdersFixture, RemovesABodyCreatedInTheSameTick) {
  const std::uint32_t networkId =
      spawnByScript(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);
  removeByScript(networkId);

  endTick();

  EXPECT_FALSE(positionOf(networkId).has_value());
}

/**
 * Given the scripts of a game attached to its world
 * When the game ends and its world goes
 * Then the scripts no longer reach any world
 */
TEST_F(EntityOrdersFixture, LetsTheScriptsGoWhenTheWorldGoes) {
  BehaviourScripts scripts{[](const std::string&) {}};
  std::optional<EntityOrders> game;
  game.emplace(partsFor(&scripts));

  game.reset();

  EXPECT_EQ(scripts.orders(), nullptr);
}

/**
 * Given the scripts of a game attached to its world
 * When a new world is built, then the old one goes, as a new game does
 * Then the scripts reach the new world
 */
TEST_F(EntityOrdersFixture, KeepsTheNewWorldWhenTheOldOneGoes) {
  BehaviourScripts scripts{[](const std::string&) {}};
  std::optional<EntityOrders> oldGame;
  oldGame.emplace(partsFor(&scripts));
  const EntityOrders newGame{partsFor(&scripts)};

  oldGame.reset();

  EXPECT_EQ(scripts.orders(), &newGame);
}

/**
 * Given a script that asked for every removal a tick allows
 * When it asks for one more in the same tick
 * Then that one is refused
 */
TEST_F(EntityOrdersFixture, RefusesARemovalPastTheLimitOfATick) {
  const std::uint32_t networkId =
      placeDirectly(DRIFTER_SHEET, WHERE_A_SCRIPT_PUTS_IT);
  for (std::size_t removal = 0; removal < SCRIPTED_REMOVALS_IN_A_TICK;
       ++removal) {
    removeByScript(networkId);
  }

  EXPECT_THROW(removeByScript(networkId), ScriptException);
}
