#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include "AllEntityTypes.hpp"
#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySpriteTestConstants.hpp"
#include "EntitySprites.hpp"
#include "EntitySpritesFixtures.hpp"
#include "EntitySpritesTestConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "SheetPixels.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

using rtype::client::AnimatedSprite;
using rtype::client::animatedSpriteOf;
using rtype::client::Animation;
using rtype::client::animationOf;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::SpriteAnimation;
using rtype::client::spriteOf;
using rtype::engine::Duration;
using rtype::game::EntityType;
using rtype::game::Layer;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

/**
 * Given every entity type the server can announce
 * When each one is looked up in the entity table
 * Then each has a sprite cut from the entity sheet
 */
TEST(EntitySprites, EveryEntityTypeHasASpriteOfTheEntitySheet) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    const std::optional<Sprite> sprite = spriteOf(type);

    ASSERT_TRUE(sprite.has_value());
    EXPECT_EQ(sprite.value_or(Sprite{}).assetId, ENTITY_SHEET_ID);
  }
}

/**
 * Given each entity type
 * When its sprite is asked for
 * Then a Bydo is drawn behind the effects, a ship and its missiles in front
 * of them, a missile of an enemy in front of everything of the playfield
 */
TEST(EntitySprites, EachTypeIsDrawnInTheLayerThatKeepsDangerInSight) {
  EXPECT_EQ(spriteOf(EntityType::Enemy).value_or(Sprite{}).layer,
            Layer::Enemies);
  EXPECT_EQ(spriteOf(EntityType::Player).value_or(Sprite{}).layer,
            Layer::Players);
  EXPECT_EQ(spriteOf(EntityType::PlayerMissile).value_or(Sprite{}).layer,
            Layer::Players);
  EXPECT_EQ(spriteOf(EntityType::EnemyMissile).value_or(Sprite{}).layer,
            Layer::EnemyMissiles);
}

/**
 * Given every entity type
 * When its animated sprite is asked for
 * Then it is drawn in the same layer as its sprite
 */
TEST(EntitySprites, AnAnimatedSpriteIsDrawnInTheLayerOfItsType) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    const std::optional<AnimatedSprite> animated = animatedSpriteOf(type, 0);

    ASSERT_TRUE(animated.has_value());
    EXPECT_EQ(animated.value_or(AnimatedSprite{}).sprite.layer,
              spriteOf(type).value_or(Sprite{}).layer);
  }
}

/**
 * Given a value past the last entity type, as a corrupted type would carry
 * When it is looked up in the entity table
 * Then the table gives no sprite
 */
TEST(EntitySprites, AValueOutsideTheEntityTypesHasNoSprite) {
  EXPECT_FALSE(spriteOf(FIRST_VALUE_AFTER_THE_LAST_TYPE).has_value());
}

/**
 * Given every pair of distinct entity types
 * When their rectangles in the entity sheet are compared
 * Then no two overlap, so no two types show the same pixels
 */
TEST(EntitySprites, NoTwoEntityTypesShareAnyPartOfTheSheet) {
  for (std::size_t first = 0; first < ALL_ENTITY_TYPES.size(); ++first) {
    for (std::size_t second = first + 1; second < ALL_ENTITY_TYPES.size();
         ++second) {
      SCOPED_TRACE(first);
      SCOPED_TRACE(second);

      EXPECT_FALSE(overlap(frameOf(ALL_ENTITY_TYPES[first]),
                           frameOf(ALL_ENTITY_TYPES[second])));
    }
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the rectangle of each entity type is laid on it
 * Then the rectangle lies inside the sheet and shows at least one visible pixel
 */
TEST(EntitySprites, EveryRectangleShowsPartOfTheShippedSheet) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));
    const SpriteFrame frame = frameOf(type);

    ASSERT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the pixels just outside the rectangle of each entity type are read
 * Then they are all transparent, so the rectangle neither cuts its sprite where
 * the sprite reaches the edge of its cell nor shows a neighbor
 */
TEST(EntitySprites, NoRectangleCutsItsSprite) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frameOf(type)));
  }
}

/**
 * Given every entity type the server can announce
 * When its animation is looked up
 * Then it has one, in a loop
 */
TEST(EntitySprites, EveryEntityTypeHasALoopingAnimation) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));

    const std::optional<SpriteAnimation> animation = animationOf(type, 0);

    ASSERT_TRUE(animation.has_value());
    EXPECT_EQ(animation.value_or(SpriteAnimation{}).animation.playback,
              Animation::Playback::Loop);
  }
}

/**
 * Given every entity type
 * When its animation is looked up for network id 0, which starts at once
 * Then it shows the frame of the sprite of the type
 */
TEST(EntitySprites, AnAnimationStartsFromTheFrameOfItsSprite) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));
    const std::optional<SpriteAnimation> animation = animationOf(type, 0);
    ASSERT_TRUE(animation.has_value());

    EXPECT_EQ(startFrameOf(animation.value_or(SpriteAnimation{})),
              frameOf(type));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When every frame of every entity animation is laid on it
 * Then each lies inside the sheet and shows at least one visible pixel
 */
TEST(EntitySprites, EveryAnimationFrameShowsPartOfTheShippedSheet) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);
  for (const SpriteFrame& frame : everyAnimationFrame()) {
    SCOPED_TRACE(frame.top);
    SCOPED_TRACE(frame.left);

    ASSERT_TRUE(liesInside(frame, sheet.getSize()));
    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
  }
}

/**
 * Given the entity sheet shipped in assets/
 * When the pixels just outside every frame of every entity animation are read
 * Then they are all transparent, so no frame cuts its sprite
 */
TEST(EntitySprites, NoAnimationFrameCutsItsSprite) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);
  for (const SpriteFrame& frame : everyAnimationFrame()) {
    SCOPED_TRACE(frame.top);
    SCOPED_TRACE(frame.left);

    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given every entity type
 * When the frame time of its animation is read
 * Then it is the one assets/README.md lists for it
 */
TEST(EntitySprites, EntityFramesLastWhatTheSheetReadmeLists) {
  const std::array<std::pair<EntityType, Duration>, 4> listed{{
      {EntityType::Player, SHIP_FRAME_TIME_LISTED},
      {EntityType::Enemy, BYDO_FRAME_TIME_LISTED},
      {EntityType::PlayerMissile, PLAYER_MISSILE_FRAME_TIME_LISTED},
      {EntityType::EnemyMissile, ENEMY_MISSILE_FRAME_TIME_LISTED},
  }};
  for (const auto& [type, frameTime] : listed) {
    SCOPED_TRACE(static_cast<int>(type));
    const std::optional<SpriteAnimation> animation = animationOf(type, 0);
    ASSERT_TRUE(animation.has_value());

    EXPECT_EQ(animation.value_or(SpriteAnimation{}).animation.frameDuration,
              frameTime);
  }
}

/**
 * Given two Bydos whose network ids follow each other, as in one wave
 * When their animations start
 * Then they show different frames, so the wave does not pulse as one block
 */
TEST(EntitySprites, BydosWithNeighbouringIdsShowDifferentFrames) {
  const std::optional<SpriteAnimation> first =
      animationOf(EntityType::Enemy, FIRST_BYDO_ID);
  const std::optional<SpriteAnimation> next =
      animationOf(EntityType::Enemy, NEXT_BYDO_ID);
  ASSERT_TRUE(first.has_value());
  ASSERT_TRUE(next.has_value());

  EXPECT_NE(startFrameOf(first.value_or(SpriteAnimation{})),
            startFrameOf(next.value_or(SpriteAnimation{})));
}

/**
 * Given every entity type and every network id from 0 to 9999
 * When the animation is looked up
 * Then it starts at an offset of zero or more and less than one cycle
 */
TEST(EntitySprites, AStartOffsetStaysWithinOneCycle) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    for (std::uint32_t networkId = 0; networkId < CHECKED_NETWORK_IDS;
         ++networkId) {
      const std::optional<SpriteAnimation> animation =
          animationOf(type, networkId);
      ASSERT_TRUE(animation.has_value());
      const SpriteAnimation started = animation.value_or(SpriteAnimation{});

      ASSERT_GE(started.elapsed, Duration::zero());
      ASSERT_LT(started.elapsed, started.animation.cycle());
    }
  }
}

/**
 * Given a value past the last entity type, as a corrupted type would carry
 * When its animation is looked up
 * Then there is none
 */
TEST(EntitySprites, AValueOutsideTheEntityTypesHasNoAnimation) {
  EXPECT_FALSE(animationOf(FIRST_VALUE_AFTER_THE_LAST_TYPE, 0).has_value());
}

/**
 * Given every entity type and every network id from 0 to 9999
 * When the look of a new entity is looked up
 * Then its sprite shows the frame its animation starts on
 */
TEST(EntitySprites, ANewEntityShowsTheFrameItsAnimationStartsOn) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    for (std::uint32_t networkId = 0; networkId < CHECKED_NETWORK_IDS;
         ++networkId) {
      const std::optional<AnimatedSprite> look =
          animatedSpriteOf(type, networkId);
      ASSERT_TRUE(look.has_value());
      const AnimatedSprite started = look.value_or(AnimatedSprite{});

      ASSERT_EQ(started.sprite.frame, startFrameOf(started.animation));
    }
  }
}

/**
 * Given every entity type and every network id from 0 to 9999
 * When the look of a new entity is looked up
 * Then it plays the animation animationOf() gives, from the same offset
 */
TEST(EntitySprites, ANewEntityPlaysTheAnimationOfItsTypeAndId) {
  for (const EntityType type : ALL_ENTITY_TYPES) {
    for (std::uint32_t networkId = 0; networkId < CHECKED_NETWORK_IDS;
         ++networkId) {
      const std::optional<AnimatedSprite> look =
          animatedSpriteOf(type, networkId);
      const std::optional<SpriteAnimation> animation =
          animationOf(type, networkId);
      ASSERT_TRUE(look.has_value());
      ASSERT_TRUE(animation.has_value());
      const SpriteAnimation played = look.value_or(AnimatedSprite{}).animation;
      const SpriteAnimation expected = animation.value_or(SpriteAnimation{});

      ASSERT_EQ(played.animation.frames.data(),
                expected.animation.frames.data());
      ASSERT_EQ(played.elapsed, expected.elapsed);
    }
  }
}

/**
 * Given a value past the last entity type, as a corrupted type would carry
 * When the look of a new entity of it is looked up
 * Then there is none
 */
TEST(EntitySprites, AValueOutsideTheEntityTypesHasNoLook) {
  EXPECT_FALSE(
      animatedSpriteOf(FIRST_VALUE_AFTER_THE_LAST_TYPE, 0).has_value());
}
