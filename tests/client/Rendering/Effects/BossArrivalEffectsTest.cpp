#include <gtest/gtest.h>
#include <vector>
#include "BossArrivalEffects.hpp"
#include "BossArrivalEffectsFixtures.hpp"
#include "BossArrivalEffectsTestConstants.hpp"
#include "EffectPreferences.hpp"
#include "EffectScene.hpp"
#include "Entity.hpp"
#include "FlashPreference.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::BossArrivalEffects;
using rtype::client::EffectPreferences;
using rtype::client::FlashPreference;
using rtype::client::joltsBetween;
using rtype::client::rumbleTraumaAt;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::NetworkIdentity;
using rtype::game::Position;

/**
 * Given the boss stage starting
 * When the steps between its start and one nanosecond before 4.4 s are
 * counted
 * Then none falls: the boss has not entered yet
 */
TEST(BossArrivalEffects, NoStepFallsBeforeTheBossEnters) {
  EXPECT_EQ(joltsBetween(ARRIVAL_STARTS, FIRST_STEP - ONE_TICK), 0);
}

/**
 * Given the boss walking in from 4.4 s
 * When the steps up to 4.4 s, then from then up to 4.85 s, are counted
 * Then one falls each time
 */
TEST(BossArrivalEffects, AStepFallsEveryFourHundredFiftyMilliseconds) {
  EXPECT_EQ(joltsBetween(FIRST_STEP - ONE_TICK, FIRST_STEP), 1);
  EXPECT_EQ(joltsBetween(FIRST_STEP, SECOND_STEP), 1);
}

/**
 * Given the boss stage
 * When every step up to long after the entry is counted
 * Then four fall: 1.8 s of walking, a step every 450 ms
 */
TEST(BossArrivalEffects, TheBossTakesFourSteps) {
  EXPECT_EQ(joltsBetween(ARRIVAL_STARTS, LONG_AFTER_THE_ENTRY),
            STEPS_OF_THE_ENTRY);
}

/**
 * Given the boss stage
 * When the rumble is read one nanosecond before 2.4 s, then at 2.4 s
 * Then the camera is still, then kept at a trauma of 0.41
 */
TEST(BossArrivalEffects, TheCameraRumblesFromTwoPointFourSeconds) {
  EXPECT_FLOAT_EQ(rumbleTraumaAt(RUMBLE_STARTS - ONE_TICK), 0.0F);
  EXPECT_FLOAT_EQ(rumbleTraumaAt(RUMBLE_STARTS), RUMBLE_TRAUMA);
}

/**
 * Given the boss stage
 * When the rumble is read at 3.8 s, as the WARNING screen ends
 * Then the camera is still
 */
TEST(BossArrivalEffects, TheRumbleStopsWithTheWarning) {
  EXPECT_FLOAT_EQ(rumbleTraumaAt(RUMBLE_STOPS), 0.0F);
}

/**
 * Given the effects of an arrival, flashes allowed
 * When the time goes over the first step of the boss
 * Then the camera takes a trauma of 0.58
 */
TEST(BossArrivalEffects, AStepShakesTheCamera) {
  EffectScene scene;
  const BossArrivalEffects arrival(scene.effects().tools());

  arrival.advance(FIRST_STEP - ONE_TICK, FIRST_STEP);

  EXPECT_NEAR(scene.effects().tools().camera.get().trauma(), STEP_TRAUMA,
              TRAUMA_TOLERANCE);
}

/**
 * Given the effects of an arrival, flashes reduced
 * When the time goes over the first step of the boss
 * Then the camera takes no trauma
 */
TEST(BossArrivalEffects, WithFlashesReducedTheCameraStaysStill) {
  EffectScene scene;
  scene.advance(Duration::zero(),
                EffectPreferences{.flashes = FlashPreference::Reduced});
  const BossArrivalEffects arrival(scene.effects().tools());

  arrival.advance(FIRST_STEP - ONE_TICK, FIRST_STEP);

  EXPECT_FLOAT_EQ(scene.effects().tools().camera.get().trauma(), 0.0F);
}

/**
 * Given the boss at rest
 * When it roars
 * Then two shockwaves open from its mouth
 */
TEST(BossArrivalEffects, TheRoarOpensTwoShockwavesAtTheMouthOfTheBoss) {
  EffectScene scene;
  const Entity boss = scene.bydoAt(ROARING_BOSS_CENTER);
  scene.components().add(boss, NetworkIdentity{.networkId = ROARING_BOSS_ID});
  const BossArrivalEffects arrival(scene.effects().tools());

  arrival.playRoar(ROARING_BOSS_ID);

  const std::vector<Position> centers = shockwaveCentersOf(scene.components());
  ASSERT_EQ(centers.size(), SHOCKWAVES_OF_THE_ROAR);
  for (const Position& center : centers) {
    EXPECT_FLOAT_EQ(center.x, ROARING_BOSS_MOUTH.x);
    EXPECT_FLOAT_EQ(center.y, ROARING_BOSS_MOUTH.y);
  }
}

/**
 * Given no entity of the network id roaring
 * When that boss roars
 * Then no shockwave opens
 */
TEST(BossArrivalEffects, ARoarOfAnUnknownBossOpensNoShockwave) {
  EffectScene scene;
  const BossArrivalEffects arrival(scene.effects().tools());

  arrival.playRoar(UNKNOWN_BOSS_ID);

  EXPECT_TRUE(shockwaveCentersOf(scene.components()).empty());
}
