#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include "Anchor.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "Lifetime.hpp"
#include "Nudge.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "ScreenTestConstants.hpp"
#include "SpriteEffectScene.hpp"
#include "SpriteEffectSystemTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::Anchor;
using rtype::client::Caption;
using rtype::client::Lifetime;
using rtype::client::Nudge;
using rtype::client::Reveal;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Position;

/**
 * Given a nudge of 100 ms
 * When 100 ms pass
 * Then it is removed: the entity is drawn at its place again
 */
TEST(SpriteEffectSystem, ANudgeEndsOnceItsTimePassed) {
  SpriteEffectScene scene;
  const Entity entity = scene.add(START);
  scene.components().add(entity, Nudge{.amount = {1, 0},
                                       .flipEvery = Duration::zero(),
                                       .elapsed = Duration::zero(),
                                       .duration = EFFECT_TIME});

  scene.run(EFFECT_TIME);

  EXPECT_FALSE(scene.components().has<Nudge>(entity));
}

/**
 * Given an entity that lasts 100 ms
 * When one nanosecond less passes, then the rest
 * Then it is alive after the first step and destroyed after the second
 */
TEST(SpriteEffectSystem, ALifetimeDestroysItsEntityOnceSpent) {
  SpriteEffectScene scene;
  const Entity entity = scene.add(START);
  scene.components().add(entity, Lifetime{.remaining = EFFECT_TIME});

  scene.run(EFFECT_TIME - ONE_TICK);
  const bool aliveBefore = scene.isAlive(entity);
  scene.run(ONE_TICK);

  EXPECT_TRUE(aliveBefore);
  EXPECT_FALSE(scene.isAlive(entity));
}

/**
 * Given a muzzle flash anchored 40 units ahead of a ship
 * When the ship moves and the system runs
 * Then the flash stands 40 units ahead of where the ship now is
 */
TEST(SpriteEffectSystem, AnAnchoredEffectFollowsItsEntity) {
  SpriteEffectScene scene;
  const Entity ship = scene.add(START);
  const Entity flash = scene.add(START);
  scene.components().add(flash, Anchor{.entity = ship, .offset = NOSE_OFFSET});
  *scene.components().get<Position>(ship) = MOVED;

  scene.run(ONE_FRAME);

  const Position* where = scene.components().get<Position>(flash);
  ASSERT_NE(where, nullptr);
  EXPECT_EQ(where->x, MOVED.x + NOSE_OFFSET.x);
  EXPECT_EQ(where->y, MOVED.y + NOSE_OFFSET.y);
}

/**
 * Given the figure of a gain just shown
 * When 400 ms pass
 * Then it stands 8 pixels, 32 units, higher
 */
TEST(SpriteEffectSystem, AFigureRisesEightPixels) {
  SpriteEffectScene scene;
  const Entity figure = scene.addFigure();

  scene.run(POPUP_RISE_LENGTH);

  EXPECT_FLOAT_EQ(scene.components().get<Position>(figure)->y,
                  START.y - POPUP_RISE_UNITS);
}

/**
 * Given the figure of a gain just shown
 * When 700 ms pass, its rise and its stay, then one blink step more
 * Then it shows during the first, and is hidden during the second: it blinks
 */
TEST(SpriteEffectSystem, AFigureBlinksAfterItsStay) {
  SpriteEffectScene scene;
  const Entity figure = scene.addFigure();

  scene.run(POPUP_STILL_LENGTH);
  const bool shownAtFirst = scene.components().get<Caption>(figure)->visible;
  scene.run(POPUP_BLINK_LENGTH);

  EXPECT_TRUE(shownAtFirst);
  EXPECT_FALSE(scene.components().get<Caption>(figure)->visible);
}

/**
 * Given the figure of a gain just shown
 * When 968 ms pass
 * Then it is gone
 */
TEST(SpriteEffectSystem, AFigureGoesAfterItsLife) {
  SpriteEffectScene scene;
  const Entity figure = scene.addFigure();

  scene.run(POPUP_LIFE_LENGTH);

  EXPECT_FALSE(scene.isAlive(figure));
}

/**
 * Given the figure of a gain shown steadily, as for a player who reduced
 * flashes
 * When 700 ms pass, its rise and its stay, then one blink step more
 * Then it still shows: it does not blink
 */
TEST(SpriteEffectSystem, ASteadyFigureDoesNotBlinkAfterItsStay) {
  SpriteEffectScene scene;
  const Entity figure = scene.addSteadyFigure();

  scene.run(POPUP_STILL_LENGTH);
  scene.run(POPUP_BLINK_LENGTH);

  EXPECT_TRUE(scene.components().get<Caption>(figure)->visible);
}

/**
 * Given the figure of a gain shown steadily
 * When 968 ms pass
 * Then it is gone, as a blinking figure is
 */
TEST(SpriteEffectSystem, ASteadyFigureGoesAfterItsLife) {
  SpriteEffectScene scene;
  const Entity figure = scene.addSteadyFigure();

  scene.run(POPUP_LIFE_LENGTH);

  EXPECT_FALSE(scene.isAlive(figure));
}

/**
 * Given a ship revealing over 267 ms
 * When 267 ms pass
 * Then the reveal is removed: the ship shows whole
 */
TEST(SpriteEffectSystem, ARevealEndsAfterItsDuration) {
  SpriteEffectScene scene;
  const Entity ship = scene.add(START);
  scene.components().add(ship, Reveal{.age = Duration::zero(),
                                      .duration = MATERIALIZE_LENGTH,
                                      .leadingLine = true});

  scene.run(MATERIALIZE_LENGTH);

  EXPECT_FALSE(scene.components().has<Reveal>(ship));
}
