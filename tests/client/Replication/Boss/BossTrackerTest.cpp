#include <gtest/gtest.h>
#include <optional>
#include "BossTracker.hpp"
#include "BossTrackerFixtures.hpp"
#include "BossTrackerTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/**
 * Given a tracker that was told nothing
 * When the time since the boss stage started is asked
 * Then there is none: no boss stage started
 */
TEST(BossTracker, KnowsNoArrivalBeforeTheBossStage) {
  const BossTracker tracker;

  EXPECT_FALSE(tracker.sinceArrival(A_SECOND_IN).has_value());
}

/**
 * Given a boss stage that started one second in
 * When the time since it started is asked half a second later
 * Then it is half a second
 */
TEST(BossTracker, CountsTheTimeSinceTheBossStageStarted) {
  BossTracker tracker;
  tracker.follow(ARRIVING, A_SECOND_IN);

  EXPECT_EQ(tracker.sinceArrival(A_SECOND_AND_A_HALF_IN),
            std::optional<engine::Duration>{HALF_A_SECOND});
}

/**
 * Given the boss fighting with 240 points, then with 230
 * When the fight is read
 * Then its health is 230, the newest
 */
TEST(BossTracker, KeepsTheHealthOfTheNewestStatus) {
  BossTracker tracker;
  tracker.follow(fightingWith(TRACKED_FULL_HEALTH), AT_THE_START);

  tracker.follow(fightingWith(AFTER_ONE_HIT), A_TENTH_OF_A_SECOND_IN);

  EXPECT_EQ(tracker.fight().health, AFTER_ONE_HIT);
}

/**
 * Given the boss fighting with 240 points
 * When a hit leaves it 230
 * Then the lag starts from the 240 it had before the hit
 */
TEST(BossTracker, KeepsTheHealthBeforeAHitAsTheLag) {
  BossTracker tracker;
  tracker.follow(fightingWith(TRACKED_FULL_HEALTH), AT_THE_START);

  tracker.follow(fightingWith(AFTER_ONE_HIT), A_TENTH_OF_A_SECOND_IN);

  EXPECT_EQ(tracker.fight().lagHealth, TRACKED_FULL_HEALTH);
}

/**
 * Given a hit at 100 ms that took the boss from 240 to 230 points
 * When another hit leaves it 220 at 400 ms, within half a second of it
 * Then the lag still starts from 240
 */
TEST(BossTracker, AHitWithinHalfASecondAddsToTheSameLag) {
  BossTracker tracker;
  tracker.follow(fightingWith(TRACKED_FULL_HEALTH), AT_THE_START);
  tracker.follow(fightingWith(AFTER_ONE_HIT), A_TENTH_OF_A_SECOND_IN);

  tracker.follow(fightingWith(AFTER_TWO_HITS), FOUR_TENTHS_IN);

  EXPECT_EQ(tracker.fight().lagHealth, TRACKED_FULL_HEALTH);
}

/**
 * Given a hit at 100 ms that took the boss from 240 to 230 points
 * When another hit leaves it 220 at 600 ms, half a second after it
 * Then a new lag starts from 230
 */
TEST(BossTracker, AHitHalfASecondLaterStartsANewLag) {
  BossTracker tracker;
  tracker.follow(fightingWith(TRACKED_FULL_HEALTH), AT_THE_START);
  tracker.follow(fightingWith(AFTER_ONE_HIT), A_TENTH_OF_A_SECOND_IN);

  tracker.follow(fightingWith(AFTER_TWO_HITS), SIX_TENTHS_IN);

  EXPECT_EQ(tracker.fight().lagHealth, AFTER_ONE_HIT);
}

/**
 * Given the boss fighting with 240 points
 * When a status at one second gives it 120, half of them
 * Then gauge I is marked emptied at one second
 */
TEST(BossTracker, MarksGaugeOneEmptiedWhenTheHealthReachesHalf) {
  BossTracker tracker;
  tracker.follow(fightingWith(TRACKED_FULL_HEALTH), AT_THE_START);

  tracker.follow(fightingWith(AT_HALF_HEALTH), A_SECOND_IN);

  EXPECT_EQ(tracker.fight().phaseOneEmptiedAt,
            std::optional<engine::Duration>{A_SECOND_IN});
}

/**
 * Given a tracker that was told nothing
 * When the first status of the fight, at two seconds, gives the boss 100 of
 * its 240 points
 * Then gauge I is marked emptied as the bar appears, at two seconds
 */
TEST(BossTracker, AFirstStatusAlreadyAtHalfMarksItAtTheAppearance) {
  BossTracker tracker;

  tracker.follow(fightingWith(BELOW_HALF_HEALTH), TWO_SECONDS_IN);

  EXPECT_EQ(tracker.fight().phaseOneEmptiedAt, tracker.fight().fightingSince);
  EXPECT_EQ(tracker.fight().fightingSince,
            std::optional<engine::Duration>{TWO_SECONDS_IN});
}

}  // namespace rtype::client::testing
