#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include "ConnectedPlayer.hpp"
#include "GameInstanceTestConstants.hpp"
#include "Intent.hpp"
#include "PlayerController.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "World.hpp"

namespace rtype::server::testing {
namespace {

constexpr game::Position SOME_SHIP{.x = 200.0F, .y = 300.0F};
constexpr std::uint32_t SOME_INPUT_NUMBER = 7;

/** @returns An input numbered SOME_INPUT_NUMBER that holds @p actions. */
network::PlayerInput inputHolding(std::uint8_t actions) {
  return network::PlayerInput{.inputNumber = SOME_INPUT_NUMBER,
                              .actions = actions};
}

/**
 * Given a player with a ship that wants nothing
 * When an input holding up and shoot arrives
 * Then the Intent of the ship holds up and shoot, and nothing else
 */
TEST(PlayerController, TurnsAnInputIntoTheIntentOfTheShip) {
  game::World world;
  PlayerRoster roster;
  ConnectedPlayer& player =
      *roster.add(playerEndpoint(FIRST_PLAYER), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN);
  player.shipEntityId = world.spawnPlayer(SOME_SHIP);

  steerShipByInput(world, player, inputHolding(HOLDS_UP | HOLDS_SHOOT));

  const game::Intent* const intent = world.shipIntentOf(player.shipEntityId);
  ASSERT_NE(intent, nullptr);
  EXPECT_TRUE(intent->directions.up);
  EXPECT_FALSE(intent->directions.down);
  EXPECT_FALSE(intent->directions.left);
  EXPECT_FALSE(intent->directions.right);
  EXPECT_TRUE(intent->shooting);
}

/**
 * Given a ship whose Intent holds up
 * When an input holding nothing arrives
 * Then the Intent of the ship holds nothing: the newest input replaces the
 * last one
 */
TEST(PlayerController, ReplacesTheIntentWithTheNewestInput) {
  game::World world;
  PlayerRoster roster;
  ConnectedPlayer& player =
      *roster.add(playerEndpoint(FIRST_PLAYER), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN);
  player.shipEntityId = world.spawnPlayer(SOME_SHIP);
  steerShipByInput(world, player, inputHolding(HOLDS_UP));

  steerShipByInput(world, player, inputHolding(HOLDS_NOTHING));

  const game::Intent* const intent = world.shipIntentOf(player.shipEntityId);
  ASSERT_NE(intent, nullptr);
  EXPECT_FALSE(intent->directions.up);
}

/**
 * Given a player without a ship
 * When an input arrives
 * Then nothing happens: no ship is steered and nothing throws
 */
TEST(PlayerController, SteersNothingForAPlayerWithoutAShip) {
  game::World world;
  PlayerRoster roster;
  const ConnectedPlayer& player =
      *roster.add(playerEndpoint(FIRST_PLAYER), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN);

  steerShipByInput(world, player, inputHolding(HOLDS_UP));

  EXPECT_EQ(world.entityCount(), 0U);
}

/**
 * Given two ships whose Intent holds up and shoot, the player of the second one
 * away
 * When the away ships are silenced
 * Then the Intent of the second ship holds nothing and the first is unchanged
 */
TEST(PlayerController, SilencesTheShipOfAnAwayPlayerOnly) {
  game::World world;
  PlayerRoster roster;
  ConnectedPlayer& present =
      *roster.add(playerEndpoint(FIRST_PLAYER), std::string{PLAYER_NICKNAME},
                  SESSION_TOKEN);
  ConnectedPlayer& away =
      *roster.add(playerEndpoint(SECOND_PLAYER), std::string{PLAYER_NICKNAME},
                  OTHER_SESSION_TOKEN);
  present.shipEntityId = world.spawnPlayer(SOME_SHIP);
  away.shipEntityId = world.spawnPlayer(SOME_SHIP);
  steerShipByInput(world, present, inputHolding(HOLDS_UP | HOLDS_SHOOT));
  steerShipByInput(world, away, inputHolding(HOLDS_UP | HOLDS_SHOOT));
  away.away = true;

  silenceAwayShips(world, roster);

  const game::Intent* const presentIntent =
      world.shipIntentOf(present.shipEntityId);
  const game::Intent* const awayIntent = world.shipIntentOf(away.shipEntityId);
  ASSERT_NE(presentIntent, nullptr);
  ASSERT_NE(awayIntent, nullptr);
  EXPECT_TRUE(presentIntent->directions.up);
  EXPECT_TRUE(presentIntent->shooting);
  EXPECT_FALSE(awayIntent->directions.up);
  EXPECT_FALSE(awayIntent->shooting);
}

}  // namespace
}  // namespace rtype::server::testing
