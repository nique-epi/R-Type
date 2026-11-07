#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameEnded.hpp"
#include "GameFlowFixture.hpp"
#include "GameFlowTestConstants.hpp"
#include "GameState.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::network::GameEnded;
using rtype::network::WireGameOutcome;
using rtype::server::GameState;
using rtype::server::testing::A_SECOND_OF_TICKS;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::GameFlowFixture;
using rtype::server::testing::GameFlowOneWaveLevelFixture;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::TICKS_TO_FIND_THE_SKY_CLEAR;

/**
 * Given a game on a level of one wave, every Bydo of the wave in the world
 * When ticks run while the Bydos stay in the world
 * Then the game goes on: the level ends once the sky is clear
 */
TEST_F(GameFlowOneWaveLevelFixture, PlaysOnWhileABydoOfTheLastWaveIsInPlay) {
  startGameWith({FIRST_PLAYER});

  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE + A_SECOND_OF_TICKS);

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game on a level of one wave whose Bydos are killed as they appear
 * When the ticks before the last Bydo appears run
 * Then the game goes on: a wave does not end before all its Bydos came
 */
TEST_F(GameFlowOneWaveLevelFixture, PlaysOnUntilTheLastBydoOfTheLastWaveCame) {
  startGameWith({FIRST_PLAYER});

  for (std::size_t index = 0; index < TICKS_TO_THE_WHOLE_FIRST_WAVE - 1;
       ++index) {
    tick();
    clearTheBydos();
  }

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game on a level of one wave, every Bydo of the wave appeared
 * When the Bydos are all killed, then ticks run
 * Then the game finishes: the last wave is over
 */
TEST_F(GameFlowOneWaveLevelFixture, FinishesOnceTheLastWaveIsOver) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();

  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR + 1);

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a game on a level of one wave, won by its player
 * When the first tick of the finished game passes
 * Then the player is sent a GameEnded that says Victory
 */
TEST_F(GameFlowOneWaveLevelFixture, TellsThePlayersTheGameWasWon) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();
  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR + 1);

  runTicks(1);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Victory);
}

/**
 * Given a game on a level of one wave, whose player loses every life
 * When the first tick of the finished game passes
 * Then the player is sent a GameEnded that says Defeat
 */
TEST_F(GameFlowOneWaveLevelFixture, TellsThePlayersTheGameWasLost) {
  startGameWith({FIRST_PLAYER});
  takeEveryLifeOf(*roster().find(playerEndpoint(FIRST_PLAYER)));
  runTicks(1);
  ASSERT_EQ(flow().state(), GameState::Finished);

  runTicks(1);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Defeat);
}

/**
 * Given a game on a level of one wave whose last wave is over
 * When the player loses every life before the next tick
 * Then the game is lost, not won: the loss comes first
 */
TEST_F(GameFlowOneWaveLevelFixture,
       LosesAGameWhoseLastLifeGoesWithItsLastWave) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();
  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR);
  takeEveryLifeOf(*roster().find(playerEndpoint(FIRST_PLAYER)));

  runTicks(2);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Defeat);
}

/**
 * Given a game on the default level, which is long
 * When every Bydo of the first wave appears and is killed, then ticks run
 * Then the game goes on: the next wave is coming
 */
TEST_F(GameFlowFixture, PlaysOnAfterAWaveBeforeTheLast) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);
  clearTheBydos();

  runTicks(TICKS_TO_FIND_THE_SKY_CLEAR + 1);

  EXPECT_EQ(flow().state(), GameState::InProgress);
}
