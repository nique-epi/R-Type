#include <gtest/gtest.h>
#include <cstdint>
#include "AllEntityTypesConstants.hpp"
#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "EntitySprites.hpp"
#include "EntitySpritesFixtures.hpp"
#include "EntitySpritesTestConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Playback.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

using rtype::client::AnimatedSprite;
using rtype::client::animatedSpriteOf;
using rtype::client::Animation;
using rtype::client::enragedIdleAnimationOf;
using rtype::client::idleAnimationOf;
using rtype::client::layerOf;
using rtype::client::Playback;
using rtype::client::SpriteAnimation;
using rtype::engine::Duration;
using rtype::game::EntityType;
using rtype::game::Layer;

/**
 * Given each side
 * When the layer of its entities is asked for
 * Then an enemy is drawn behind the effects, a ship and its missiles in front
 * of them, a missile of an enemy in front of everything of the playfield
 */
TEST(EntitySprites, EachSideIsDrawnInTheLayerThatKeepsDangerInSight) {
  EXPECT_EQ(layerOf(EntityType::Enemy), Layer::Enemies);
  EXPECT_EQ(layerOf(EntityType::Player), Layer::Players);
  EXPECT_EQ(layerOf(EntityType::PlayerMissile), Layer::Players);
  EXPECT_EQ(layerOf(EntityType::EnemyMissile), Layer::EnemyMissiles);
}

/**
 * Given every side
 * When a new entity of that side is given the test appearance
 * Then it is drawn in the layer of its side, whatever its appearance
 */
TEST(EntitySprites, ANewEntityIsDrawnInTheLayerOfItsSide) {
  for (const EntityType side : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(side));

    const AnimatedSprite look = animatedSpriteOf(testAppearance(), side, 0);

    EXPECT_EQ(look.sprite.layer, layerOf(side));
  }
}

/**
 * Given the test appearance, drawn from sprites/test.png
 * When a new entity is given it
 * Then its sprite is cut from that sheet
 */
TEST(EntitySprites, ANewEntityIsCutFromTheSheetOfItsAppearance) {
  const AnimatedSprite look =
      animatedSpriteOf(testAppearance(), EntityType::Enemy, 0);

  EXPECT_EQ(look.sprite.assetId, TEST_SHEET_ID);
}

/**
 * Given the test appearance
 * When a new entity is given it
 * Then it plays the idle frames of the appearance, 250 ms each, in a loop
 */
TEST(EntitySprites, ANewEntityLoopsTheIdleFramesOfItsAppearance) {
  const Animation played =
      animatedSpriteOf(testAppearance(), EntityType::Enemy, 0)
          .animation.animation;

  EXPECT_EQ(played.frames.data(), testAppearance().idleFrames.data());
  EXPECT_EQ(played.frames.size(), TEST_IDLE_FRAMES.size());
  EXPECT_EQ(played.frameDuration, TEST_FRAME_TIME);
  EXPECT_EQ(played.playback, Playback::Loop);
}

/**
 * Given the test appearance
 * When an entity of network id 0, which starts at once, is given it
 * Then it shows the first idle frame
 */
TEST(EntitySprites, IdZeroStartsOnTheFirstIdleFrame) {
  const AnimatedSprite look =
      animatedSpriteOf(testAppearance(), EntityType::Enemy, 0);

  EXPECT_EQ(look.sprite.frame, TEST_IDLE_FRAMES.front());
}

/**
 * Given two Bydos whose network ids follow each other, as in one wave
 * When their animations start
 * Then they show different frames, so the wave does not pulse as one block
 */
TEST(EntitySprites, BydosWithNeighbouringIdsShowDifferentFrames) {
  const AnimatedSprite first =
      animatedSpriteOf(testAppearance(), EntityType::Enemy, FIRST_BYDO_ID);
  const AnimatedSprite next =
      animatedSpriteOf(testAppearance(), EntityType::Enemy, NEXT_BYDO_ID);

  EXPECT_NE(startFrameOf(first.animation), startFrameOf(next.animation));
}

/**
 * Given every network id from 0 to 9999
 * When a new entity of that id is given the test appearance
 * Then its animation starts at an offset of zero or more and less than one
 * cycle
 */
TEST(EntitySprites, AStartOffsetStaysWithinOneCycle) {
  for (std::uint32_t networkId = 0; networkId < CHECKED_NETWORK_IDS;
       ++networkId) {
    const SpriteAnimation started =
        animatedSpriteOf(testAppearance(), EntityType::Enemy, networkId)
            .animation;

    ASSERT_GE(started.elapsed, Duration::zero());
    ASSERT_LT(started.elapsed, started.animation.cycle());
  }
}

/**
 * Given every network id from 0 to 9999
 * When a new entity of that id is given the test appearance
 * Then its sprite shows the frame its animation starts on
 */
TEST(EntitySprites, ANewEntityShowsTheFrameItsAnimationStartsOn) {
  for (std::uint32_t networkId = 0; networkId < CHECKED_NETWORK_IDS;
       ++networkId) {
    const AnimatedSprite look =
        animatedSpriteOf(testAppearance(), EntityType::Enemy, networkId);

    ASSERT_EQ(look.sprite.frame, startFrameOf(look.animation));
  }
}

/**
 * Given the test appearance, which has no enraged idle frames
 * When its enraged idle animation is asked for
 * Then it is its idle animation: the same frames, the same time, a loop
 */
TEST(EntitySprites, TheEnragedIdleFallsBackToTheIdleWithoutItsFrames) {
  const Animation enraged = enragedIdleAnimationOf(testAppearance());
  const Animation idle = idleAnimationOf(testAppearance());

  EXPECT_EQ(enraged.frames.data(), idle.frames.data());
  EXPECT_EQ(enraged.frames.size(), idle.frames.size());
  EXPECT_EQ(enraged.frameDuration, idle.frameDuration);
  EXPECT_EQ(enraged.playback, Playback::Loop);
}
