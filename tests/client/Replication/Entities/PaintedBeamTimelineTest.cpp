#include <gtest/gtest.h>
#include <optional>
#include "ActionKey.hpp"
#include "EnemyAction.hpp"
#include "PaintedBeamTimelineTestConstants.hpp"
#include "PaintedBeamsScene.hpp"

using rtype::client::testing::BEAM_SIGNAL_KEY;
using rtype::client::testing::BEAM_STRIKE_KEY;
using rtype::client::testing::BEAMING_ENEMY_ID;
using rtype::client::testing::DRAWN_BEFORE_THE_STRIKE;
using rtype::client::testing::DRAWN_INTO_THE_SIGNAL;
using rtype::client::testing::DRAWN_INTO_THE_STRIKE;
using rtype::client::testing::DRAWN_JUST_INTO_THE_STRIKE;
using rtype::client::testing::FIRST_DRAWN_TICK;
using rtype::client::testing::FIVE_TICKS;
using rtype::client::testing::ONE_TICK;
using rtype::client::testing::PaintedBeamsScene;
using rtype::client::testing::SIGNAL_ANNOUNCED_AT;
using rtype::client::testing::SIX_TICKS;
using rtype::client::testing::STRIKE_ANNOUNCED_AT;
using rtype::client::testing::UNKNOWN_ENEMY_ID;
using rtype::game::ActionKey;
using rtype::network::EnemyActionPhase;

/**
 * Given the strike of an enemy announced at server tick 100
 * When the client draws tick 99.5
 * Then it shows no action yet: its beam would run ahead of the enemy drawn
 */
TEST(PaintedBeamTimeline, ShowsNoActionBeforeTheDrawnTickReachesIt) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  scene.drawTick(DRAWN_BEFORE_THE_STRIKE);

  EXPECT_FALSE(scene.beams().shown.key.has_value());
}

/**
 * Given the strike of an enemy announced at server tick 100
 * When the client draws tick 106
 * Then it shows the strike, 6 ticks into it
 */
TEST(PaintedBeamTimeline, ShowsAnActionOnceTheDrawnTickReachesIt) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  scene.drawTick(DRAWN_INTO_THE_STRIKE);

  EXPECT_EQ(scene.beams().shown.key.value_or(ActionKey{}), BEAM_STRIKE_KEY);
  EXPECT_EQ(scene.beams().shown.since, SIX_TICKS);
}

/**
 * Given the signal of an enemy announced at tick 90 and its strike at 100
 * When the client draws tick 95
 * Then it shows the signal, 5 ticks into it
 */
TEST(PaintedBeamTimeline, ShowsTheActionTheDrawnTickIsIn) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Signal,
                 SIGNAL_ANNOUNCED_AT);
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  scene.drawTick(DRAWN_INTO_THE_SIGNAL);

  EXPECT_EQ(scene.beams().shown.key.value_or(ActionKey{}), BEAM_SIGNAL_KEY);
  EXPECT_EQ(scene.beams().shown.since, FIVE_TICKS);
}

/**
 * Given the signal of an enemy announced at tick 90 and its strike at 100
 * When the client draws tick 101
 * Then it shows the strike, 1 tick into it, and keeps no action announced
 */
TEST(PaintedBeamTimeline, ShowsTheNewestDueActionAndForgetsTheOlder) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Signal,
                 SIGNAL_ANNOUNCED_AT);
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  scene.drawTick(DRAWN_JUST_INTO_THE_STRIKE);

  EXPECT_EQ(scene.beams().shown.key.value_or(ActionKey{}), BEAM_STRIKE_KEY);
  EXPECT_EQ(scene.beams().shown.since, ONE_TICK);
  EXPECT_TRUE(scene.beams().announced.empty());
}

/**
 * Given the strike of an enemy shown since tick 100
 * When the client draws tick 106 with nothing more announced
 * Then the strike is still shown, its time counted on: 6 ticks
 */
TEST(PaintedBeamTimeline, CountsTheTimeOfTheActionShown) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);
  scene.drawTick(DRAWN_JUST_INTO_THE_STRIKE);

  scene.drawTick(DRAWN_INTO_THE_STRIKE);

  EXPECT_EQ(scene.beams().shown.since, SIX_TICKS);
}

/**
 * Given an enemy whose beams are painted
 * When an action of an enemy the client does not have is announced
 * Then the enemy records nothing
 */
TEST(PaintedBeamTimeline, AnActionOfAnotherEnemyIsNotRecorded) {
  PaintedBeamsScene scene;

  scene.announce(UNKNOWN_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  EXPECT_TRUE(scene.beams().announced.empty());
}

/**
 * Given a strike announced before the client estimates the server tick
 * When the client draws its first tick
 * Then it shows the strike: an action without a stamp is not held back
 */
TEST(PaintedBeamTimeline, WithoutAnEstimateAnActionShowsAtOnce) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike, std::nullopt);

  scene.drawTick(FIRST_DRAWN_TICK);

  EXPECT_EQ(scene.beams().shown.key.value_or(ActionKey{}), BEAM_STRIKE_KEY);
}

/**
 * Given the strike of an enemy announced at tick 100
 * When the client draws no tick yet
 * Then nothing is shown
 */
TEST(PaintedBeamTimeline, ShowsNothingWhileNoTickIsDrawn) {
  PaintedBeamsScene scene;
  scene.announce(BEAMING_ENEMY_ID, EnemyActionPhase::Strike,
                 STRIKE_ANNOUNCED_AT);

  scene.drawTick(std::nullopt);

  EXPECT_FALSE(scene.beams().shown.key.has_value());
}
