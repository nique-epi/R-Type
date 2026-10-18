#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "ShowcaseTestConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteShowcase.hpp"
#include "SpriteShowcaseConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::animationOf;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::BYDO_FRAMES;
using rtype::client::EffectSpawner;
using rtype::client::ENEMY_MUZZLE_FLASH_FRAME;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::SHOWCASE_BYDO_EXPLODES_AT;
using rtype::client::SHOWCASE_LOOP;
using rtype::client::SHOWCASE_RETURN_AT;
using rtype::client::SHOWCASE_SHOTS_AT;
using rtype::client::SPRITE_SHOWCASE;
using rtype::client::SpriteAnimation;
using rtype::client::SpriteShowcase;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::EntityType;
using rtype::game::Layer;
using rtype::game::Position;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

namespace {

constexpr std::size_t SHOWCASE_BYDOS = 3;
constexpr std::size_t FLASHES_OF_TWO_SHOTS = 4;
constexpr Duration BEFORE_THE_END = std::chrono::milliseconds(100);
constexpr Duration ACROSS_THE_END = std::chrono::milliseconds(400);
constexpr int MANY_LOOPS = 10;

/** @brief The showcase alone in its registries, with its spawner of
 * effects. */
class ShowcaseScene {
 public:
  SpriteShowcase& showcase() { return showcase_; }

  /** @returns The frames of the sprites of a layer, in index order. */
  std::vector<SpriteFrame> framesIn(Layer layer) {
    std::vector<SpriteFrame> frames;
    components_.forEach<Position, Sprite>(
        [&frames, layer](Entity, const Position&, const Sprite& sprite) {
          if (sprite.layer == layer) {
            frames.push_back(sprite.frame);
          }
        });
    return frames;
  }

  /** @returns How many effects show a muzzle flash. */
  std::size_t muzzleFlashes() {
    const std::vector<SpriteFrame> effects = framesIn(Layer::Effects);
    return static_cast<std::size_t>(
        std::ranges::count_if(effects, [](const SpriteFrame& frame) noexcept {
          return frame == PLAYER_MUZZLE_FLASH_FRAME ||
                 frame == ENEMY_MUZZLE_FLASH_FRAME;
        }));
  }

  /** @returns How many entities show a frame of the Bydo. */
  std::size_t bydos() {
    const std::vector<SpriteFrame> entities = framesIn(Layer::Entities);
    return static_cast<std::size_t>(
        std::ranges::count_if(entities, [](const SpriteFrame& frame) noexcept {
          return std::ranges::find(BYDO_FRAMES, frame) != BYDO_FRAMES.end();
        }));
  }

  /** @returns How many entities carry an animation. */
  std::size_t animatedEntities() {
    std::size_t animated = 0;
    components_.forEach<SpriteAnimation, SpriteAnimation>(
        [&animated](Entity, const SpriteAnimation&, const SpriteAnimation&) {
          ++animated;
        });
    return animated;
  }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_};
  SpriteShowcase showcase_{entities_, components_, effects_};
};

}  // namespace

/**
 * Given a showcase just built
 * When its entities are read
 * Then each entity type shows a frame of its animation at least once
 */
TEST(SpriteShowcase, ShowsEachEntityTypeAtLeastOnce) {
  ShowcaseScene scene;
  const std::vector<SpriteFrame> shown = scene.framesIn(Layer::Entities);
  for (const EntityType type : ALL_ENTITY_TYPES) {
    SCOPED_TRACE(static_cast<int>(type));
    const std::optional<SpriteAnimation> animation = animationOf(type, 0);
    ASSERT_TRUE(animation.has_value());

    EXPECT_TRUE(std::ranges::any_of(
        shown, [&animation](const SpriteFrame& frame) noexcept {
          return std::ranges::find(animation->animation.frames, frame) !=
                 animation->animation.frames.end();
        }));
  }
}

/**
 * Given a showcase just built
 * When its Bydos are counted
 * Then there are three, so their pulses can be seen out of step
 */
TEST(SpriteShowcase, ShowsThreeBydos) {
  ShowcaseScene scene;

  EXPECT_EQ(scene.bydos(), SHOWCASE_BYDOS);
}

/**
 * Given a showcase just built
 * When its animations are counted
 * Then every entity of the showcase is animated
 */
TEST(SpriteShowcase, EveryEntityOfTheShowcaseIsAnimated) {
  ShowcaseScene scene;

  EXPECT_EQ(scene.animatedEntities(), SPRITE_SHOWCASE.size());
}

/**
 * Given a showcase just built
 * When it advances to just before its first step
 * Then no effect has played
 */
TEST(SpriteShowcase, NothingPlaysBeforeTheFirstStep) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_SHOTS_AT - ONE_TICK);

  EXPECT_TRUE(scene.framesIn(Layer::Effects).empty());
}

/**
 * Given a showcase just built
 * When it advances to the shots
 * Then both missiles flash
 */
TEST(SpriteShowcase, BothMissilesFlashAtTheShots) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_SHOTS_AT);

  EXPECT_EQ(scene.muzzleFlashes(), MUZZLE_FLASHES_PER_SHOT);
}

/**
 * Given a showcase that played its shots
 * When it advances to just before the end of its loop
 * Then the shots have not played again
 */
TEST(SpriteShowcase, AStepPlaysOncePerLoop) {
  ShowcaseScene scene;
  scene.showcase().advance(SHOWCASE_SHOTS_AT);

  scene.showcase().advance(SHOWCASE_LOOP - SHOWCASE_SHOTS_AT - ONE_TICK);

  EXPECT_EQ(scene.muzzleFlashes(), MUZZLE_FLASHES_PER_SHOT);
}

/**
 * Given a showcase that played a whole loop
 * When it advances to the shots again
 * Then the shots have played a second time
 */
TEST(SpriteShowcase, TheLoopStartsAgainAfterItsLength) {
  ShowcaseScene scene;
  scene.showcase().advance(SHOWCASE_LOOP);

  scene.showcase().advance(SHOWCASE_SHOTS_AT);

  EXPECT_EQ(scene.muzzleFlashes(), FLASHES_OF_TWO_SHOTS);
}

/**
 * Given a showcase just built
 * When one step of ten loops comes, as after a stall
 * Then it plays one loop only: the shots once
 */
TEST(SpriteShowcase, ALongStepPlaysOneLoopAtMost) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_LOOP * MANY_LOOPS);

  EXPECT_EQ(scene.muzzleFlashes(), MUZZLE_FLASHES_PER_SHOT);
}

/**
 * Given a showcase 100 ms before the end of its loop
 * When a step of 400 ms crosses the end of the loop
 * Then the shots of the next loop play
 */
TEST(SpriteShowcase, AStepAcrossTheEndOfTheLoopPlaysTheNextLoop) {
  ShowcaseScene scene;
  scene.showcase().advance(SHOWCASE_LOOP - BEFORE_THE_END);

  scene.showcase().advance(ACROSS_THE_END);

  EXPECT_EQ(scene.muzzleFlashes(), FLASHES_OF_TWO_SHOTS);
}

/**
 * Given a showcase just built
 * When it advances to the explosion of the first Bydo
 * Then two Bydos are left
 */
TEST(SpriteShowcase, TheFirstBydoDisappearsWhenItExplodes) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_BYDO_EXPLODES_AT);

  EXPECT_EQ(scene.bydos(), SHOWCASE_BYDOS - 1);
}

/**
 * Given a showcase just built
 * When it advances to the explosion of the first Bydo
 * Then an effect shows the first frame of the Bydo explosion
 */
TEST(SpriteShowcase, TheFirstBydoLeavesAnExplosion) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_BYDO_EXPLODES_AT);

  const std::vector<SpriteFrame> effects = scene.framesIn(Layer::Effects);
  EXPECT_NE(std::ranges::find(effects, BYDO_EXPLOSION_FRAME), effects.end());
}

/**
 * Given a showcase just built
 * When it advances to the return
 * Then every entity of the showcase is drawn again
 */
TEST(SpriteShowcase, TheShipAndTheBydoComeBack) {
  ShowcaseScene scene;

  scene.showcase().advance(SHOWCASE_RETURN_AT);

  EXPECT_EQ(scene.framesIn(Layer::Entities).size(), SPRITE_SHOWCASE.size());
}
