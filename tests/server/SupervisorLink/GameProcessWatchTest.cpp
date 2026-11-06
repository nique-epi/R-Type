#include <gtest/gtest.h>
#include "GameProcessWatch.hpp"
#include "GameProcessWatchTestConstants.hpp"

using rtype::server::GameProcessWatch;

/**
 * Given a watch of a process whose supervisor spoke just now, with a player
 * When 4.999 seconds pass with the supervisor silent
 * Then it does not ask to stop
 */
TEST(GameProcessWatch, DoesNotStopJustUnderTheSilenceLimit) {
  GameProcessWatch watch;
  watch.tick(ONE_MILLISECOND, ONE_PLAYER);
  watch.heardSupervisor();

  watch.tick(JUST_UNDER_FIVE_SECONDS, ONE_PLAYER);

  EXPECT_FALSE(watch.reasonToStop().has_value());
}

/**
 * Given a watch of a process with a player
 * When 5 seconds pass with the supervisor silent
 * Then it asks to stop
 */
TEST(GameProcessWatch, StopsWhenTheSupervisorIsSilentForFiveSeconds) {
  GameProcessWatch watch;

  watch.tick(FIVE_SECONDS, ONE_PLAYER);

  EXPECT_TRUE(watch.reasonToStop().has_value());
}

/**
 * Given a watch whose supervisor was silent for 4.999 seconds
 * When the supervisor speaks, then 4.999 seconds pass
 * Then it does not ask to stop: the silence counts from the last word
 */
TEST(GameProcessWatch, TheSilenceCountsFromTheLastWordOfTheSupervisor) {
  GameProcessWatch watch;
  watch.tick(JUST_UNDER_FIVE_SECONDS, ONE_PLAYER);
  watch.heardSupervisor();

  watch.tick(JUST_UNDER_FIVE_SECONDS, ONE_PLAYER);

  EXPECT_FALSE(watch.reasonToStop().has_value());
}

/**
 * Given a process that had a player, whose supervisor keeps speaking
 * When the player is gone for 9.999 seconds
 * Then it does not ask to stop
 */
TEST(GameProcessWatch, WaitsForAPlayerThatLeftJustUnderTheGrace) {
  GameProcessWatch watch;
  watch.tick(ONE_MILLISECOND, ONE_PLAYER);
  watch.tick(JUST_UNDER_TEN_SECONDS, NO_PLAYER);
  watch.heardSupervisor();

  EXPECT_FALSE(watch.reasonToStop().has_value());
}

/**
 * Given a process that had a player, whose supervisor keeps speaking
 * When the player is gone for 10 seconds
 * Then it asks to stop
 */
TEST(GameProcessWatch, StopsWhenItsPlayersAreGoneForTenSeconds) {
  GameProcessWatch watch;
  watch.tick(ONE_MILLISECOND, ONE_PLAYER);
  watch.tick(TEN_SECONDS, NO_PLAYER);
  watch.heardSupervisor();

  EXPECT_TRUE(watch.reasonToStop().has_value());
}

/**
 * Given a process that had a player who left for 9.999 seconds
 * When a player comes back, then 9.999 seconds pass without one
 * Then it does not ask to stop: the grace starts again
 */
TEST(GameProcessWatch, ThePlayerGraceStartsAgainWhenAPlayerComesBack) {
  GameProcessWatch watch;
  watch.tick(ONE_MILLISECOND, ONE_PLAYER);
  watch.tick(JUST_UNDER_TEN_SECONDS, NO_PLAYER);
  watch.heardSupervisor();
  watch.tick(ONE_MILLISECOND, ONE_PLAYER);
  watch.heardSupervisor();

  watch.tick(JUST_UNDER_TEN_SECONDS, NO_PLAYER);
  watch.heardSupervisor();

  EXPECT_FALSE(watch.reasonToStop().has_value());
}

/**
 * Given a process nobody joined, whose supervisor keeps speaking
 * When 29.999 seconds pass
 * Then it does not ask to stop
 */
TEST(GameProcessWatch, WaitsForAFirstPlayerJustUnderThirtySeconds) {
  GameProcessWatch watch;
  watch.tick(JUST_UNDER_THIRTY_SECONDS, NO_PLAYER);
  watch.heardSupervisor();

  EXPECT_FALSE(watch.reasonToStop().has_value());
}

/**
 * Given a process nobody joined, whose supervisor keeps speaking
 * When 30 seconds pass
 * Then it asks to stop
 */
TEST(GameProcessWatch, StopsWhenNobodyCameInThirtySeconds) {
  GameProcessWatch watch;
  watch.tick(THIRTY_SECONDS, NO_PLAYER);
  watch.heardSupervisor();

  EXPECT_TRUE(watch.reasonToStop().has_value());
}

/**
 * Given a process nobody joined for 10 seconds, then a player who stays
 * When 30 more seconds pass with that player there
 * Then it does not ask to stop, whatever the time since the start
 */
TEST(GameProcessWatch, DoesNotStopWhileAPlayerIsThere) {
  GameProcessWatch watch;
  watch.tick(TEN_SECONDS, NO_PLAYER);
  watch.heardSupervisor();
  watch.tick(FIVE_SECONDS, ONE_PLAYER);
  watch.heardSupervisor();

  watch.tick(THIRTY_SECONDS, ONE_PLAYER);
  watch.heardSupervisor();

  EXPECT_FALSE(watch.reasonToStop().has_value());
}
