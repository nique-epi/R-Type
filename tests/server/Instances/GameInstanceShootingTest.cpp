#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "BydoKind.hpp"
#include "BydoPointsConstants.hpp"
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceShootingFixture.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "SentDatagrams.hpp"
#include "Spawning/BydoGunnerTestConstants.hpp"
#include "Spawning/BydoSpawnerTestConstants.hpp"
#include "Spawning/MissileLauncherTestConstants.hpp"
#include "World.hpp"

namespace rtype::server::testing {
namespace {

/**
 * Given a game in progress whose first player holds shoot
 * When the tick that reads the input runs
 * Then every player is sent the EntitySpawned of one missile
 */
TEST_F(GameInstanceShootingFixture, FiresWhileInProgress) {
  startGame();
  holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);

  tick();

  EXPECT_EQ(missilesSentTo(FIRST_PLAYER).size(), 1U);
  EXPECT_EQ(missilesSentTo(SECOND_PLAYER).size(), 1U);
}

/**
 * Given a game in progress whose first player holds right and not shoot
 * When ticks run
 * Then no missile is fired
 */
TEST_F(GameInstanceShootingFixture, FiresNothingWhileShootIsNotHeld) {
  startGame();
  holdForNextTick(FIRST_PLAYER, HOLDS_RIGHT);

  runTicks(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_TRUE(missilesSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a game in progress whose players sent no input yet
 * When ticks run
 * Then no missile is fired
 */
TEST_F(GameInstanceShootingFixture, FiresNothingBeforeTheFirstInput) {
  startGame();

  runTicks(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_TRUE(missilesSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a game waiting for players whose first player holds shoot
 * When ticks run
 * Then no missile is fired: ships fire only while the game is in progress
 */
TEST_F(GameInstanceShootingFixture, FiresNothingWhileWaitingForPlayers) {
  holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);

  runTicks(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_TRUE(missilesSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a finished game whose first player holds shoot
 * When ticks run
 * Then no missile is fired
 */
TEST_F(GameInstanceShootingFixture, FiresNothingWhileFinished) {
  startGame();
  game().finish();
  holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);

  runTicks(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_TRUE(missilesSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a game in progress whose first player sends DATAGRAMS_PER_TICK inputs
 * holding shoot before every tick, as a modified client could
 * When 20 ticks run
 * Then one missile only was fired
 */
TEST_F(GameInstanceShootingFixture, ManyInputsPerTickFireNoFaster) {
  startGame();

  for (std::size_t tickIndex = 0; tickIndex < TICKS_BETWEEN_TWO_SHOTS;
       ++tickIndex) {
    for (std::size_t input = 0; input < DATAGRAMS_PER_TICK; ++input) {
      holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);
    }
    tick();
  }

  EXPECT_EQ(missilesSentTo(FIRST_PLAYER).size(), 1U);
}

/**
 * Given a game in progress whose first player fired one missile, then
 * released shoot
 * When ticks run until just before the first Bydo appears, nearly two seconds
 * Then the missile left on the right: it is no longer in the world, and its
 * removal was announced
 */
TEST_F(GameInstanceShootingFixture, RemovesAMissileThatLeftOnTheRight) {
  recordDestructions();
  startGame();
  holdForNextTick(FIRST_PLAYER, HOLDS_SHOOT);
  tick();
  ASSERT_EQ(missilesSentTo(FIRST_PLAYER).size(), 1U);
  const std::uint32_t missile = missilesSentTo(FIRST_PLAYER).front().entityId;
  holdForNextTick(FIRST_PLAYER, HOLDS_NOTHING);

  runTicksDispatching(TICKS_PER_BYDO_INTERVAL - 2);

  EXPECT_TRUE(wasDestroyed(missile, game::DestructionCause::Removed));
  EXPECT_EQ(game().world().entityCount(), 2U);
}

/**
 * Given a game in progress whose first player holds shoot at a Drifter on its
 * row
 * When ticks run until just before the first Bydo appears
 * Then the Drifter was destroyed, with an explosion
 */
TEST_F(GameInstanceShootingFixture, DestroysABydoWithItsMissiles) {
  recordDestructions();
  startGame();
  const std::uint32_t drifter = aimAtADrifter();

  runTicksDispatching(TICKS_PER_BYDO_INTERVAL - 1);

  EXPECT_TRUE(wasDestroyed(drifter, game::DestructionCause::Killed));
}

/**
 * Given a game in progress whose first player holds shoot at a Drifter on its
 * row
 * When ticks run until just before the first Bydo appears
 * Then the first player earned the points of a Drifter
 */
TEST_F(GameInstanceShootingFixture, CreditsTheBydoToThePlayerWhoFired) {
  startGame();
  static_cast<void>(aimAtADrifter());

  runTicksDispatching(TICKS_PER_BYDO_INTERVAL - 1);

  const ConnectedPlayer* const player =
      game().roster().find(playerEndpoint(FIRST_PLAYER));
  ASSERT_NE(player, nullptr);
  EXPECT_EQ(player->score, game::DRIFTER_POINTS);
}

/**
 * Given a second game, started after a first one finished
 * When the first player holds shoot at a Drifter on its row until just before
 * the first Bydo appears
 * Then the Drifter was destroyed: the collisions are judged in the world of
 * the second game
 */
TEST_F(GameInstanceShootingFixture, DestroysABydoInTheNextGameToo) {
  recordDestructions();
  startGame();
  game().finish();
  runTicksDispatching(FINISHED_STATE_TICKS);
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    sendReady({.player = player,
               .ready = true,
               .sequence = SECOND_SEQUENCE,
               .sessionToken = PLAYER_SESSION_TOKENS.at(player)});
  }
  tick();
  ASSERT_EQ(game().state(), GameState::InProgress);
  const std::uint32_t drifter = aimAtADrifter();

  runTicksDispatching(TICKS_PER_BYDO_INTERVAL - 1);

  EXPECT_TRUE(wasDestroyed(drifter, game::DestructionCause::Killed));
}

/**
 * Given a game in progress and a Drifter far ahead of the ships
 * When the ticks of one and a half seconds run
 * Then every player is sent the EntitySpawned of one missile of an enemy
 */
TEST_F(GameInstanceShootingFixture, BydosFireWhileInProgress) {
  startGame();
  game().world().spawnBydo(BYDO_FAR_AHEAD_OF_THE_SHIPS,
                           game::BydoKind::Drifter);

  runTicksDispatching(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    EXPECT_EQ(enemyMissilesSentTo(player).size(), 1U);
  }
}

}  // namespace
}  // namespace rtype::server::testing
