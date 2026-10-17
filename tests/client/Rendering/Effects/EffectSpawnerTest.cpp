#include <gtest/gtest.h>
#include <vector>
#include "AllEntityTypes.hpp"
#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectLengths.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "Sprite.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

using rtype::client::AnimationSystem;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::BYDO_EXPLOSION_WITHOUT_FLASH_FRAME;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectSpawner;
using rtype::client::ENEMY_MUZZLE_FLASH_FRAME;
using rtype::client::FlashPreference;
using rtype::client::HitFlash;
using rtype::client::IMPACT_FRAME;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::SHIP_EXPLOSION_FRAME;
using rtype::client::SpriteTone;
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

constexpr Position WHERE_IT_DIED{.x = 640.0F, .y = 360.0F};
constexpr SpriteFrame TARGET_FRAME{
    .left = 0, .top = 0, .width = 24, .height = 24};

/** @brief Registries with a spawner of effects, and the system that plays
 * them. */
class EffectScene {
 public:
  EffectSpawner& effects() { return effects_; }

  void play(Duration elapsed) { animations_.update(components_, elapsed); }

  /** @returns The entities drawn in the Effects layer, in index order. */
  std::vector<Entity> shownEffects() {
    std::vector<Entity> shown;
    components_.forEach<Position, Sprite>(
        [&shown](Entity entity, const Position&, const Sprite& sprite) {
          if (sprite.layer == Layer::Effects) {
            shown.push_back(entity);
          }
        });
    return shown;
  }

  [[nodiscard]] Sprite drawnSprite(Entity entity) {
    const Sprite* sprite = components_.get<Sprite>(entity);
    return sprite != nullptr ? *sprite : Sprite{};
  }

  [[nodiscard]] Position positionOf(Entity entity) {
    const Position* position = components_.get<Position>(entity);
    return position != nullptr ? *position : Position{};
  }

  /** @returns A drawn entity, as a Bydo the player may hit. */
  Entity target() {
    const Entity entity = entities_.create();
    components_.add(entity, WHERE_IT_DIED);
    components_.add(entity, Sprite{.assetId = "target",
                                   .frame = TARGET_FRAME,
                                   .layer = Layer::Entities});
    return entity;
  }

  /** @returns The flash of an entity, or nothing when it has none. */
  [[nodiscard]] const HitFlash* flashOf(Entity entity) {
    return components_.get<HitFlash>(entity);
  }

  EntityRegistry& entities() { return entities_; }
  ComponentRegistry& components() { return components_; }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_};
  AnimationSystem animations_;
};

}  // namespace

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then one effect shows the first frame of the Bydo explosion
 */
TEST(EffectSpawner, AKilledBydoBurstsIntoDrops) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).frame, BYDO_EXPLOSION_FRAME);
}

/**
 * Given a spawner of effects
 * When a ship is killed
 * Then one effect shows the first frame of the chain of fireballs
 */
TEST(EffectSpawner, AKilledShipPlaysTheChainOfFireballs) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Player, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).frame, SHIP_EXPLOSION_FRAME);
}

/**
 * Given a spawner of effects
 * When a missile of a player, or of an enemy, is destroyed on a target
 * Then one effect shows the first frame of the impact
 */
TEST(EffectSpawner, AKilledMissileBurstsIntoAnImpact) {
  for (const EntityType missile :
       {EntityType::PlayerMissile, EntityType::EnemyMissile}) {
    SCOPED_TRACE(static_cast<int>(missile));
    EffectScene scene;

    scene.effects().playExplosion(missile, WHERE_IT_DIED);

    const std::vector<Entity> shown = scene.shownEffects();
    ASSERT_EQ(shown.size(), 1U);
    EXPECT_EQ(scene.drawnSprite(shown.front()).frame, IMPACT_FRAME);
  }
}

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then its explosion is drawn from the effect sheet
 */
TEST(EffectSpawner, AnEffectIsDrawnFromTheEffectSheet) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).assetId, EFFECT_SHEET_ID);
}

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then its explosion is drawn in the Effects layer, in front of the entities
 */
TEST(EffectSpawner, AnEffectIsDrawnInFrontOfTheEntities) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  EXPECT_EQ(scene.shownEffects().size(), 1U);
}

/**
 * Given a spawner of effects
 * When a Bydo is killed at a position
 * Then its explosion is centered on that position
 */
TEST(EffectSpawner, AnEffectIsCenteredWhereTheEntityDied) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.positionOf(shown.front()).x, WHERE_IT_DIED.x);
  EXPECT_EQ(scene.positionOf(shown.front()).y, WHERE_IT_DIED.y);
}

/**
 * Given a Bydo explosion just started
 * When one nanosecond less than 420 ms passes
 * Then the explosion is still drawn
 */
TEST(EffectSpawner, AnExplosionStaysUntilItsLastFrameHasShown) {
  EffectScene scene;
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.play(BYDO_EXPLOSION_LENGTH - ONE_TICK);

  EXPECT_EQ(scene.shownEffects().size(), 1U);
}

/**
 * Given a Bydo explosion just started
 * When 420 ms pass, its six frames of 70 ms as on the animation page
 * Then no effect is left
 */
TEST(EffectSpawner, AnExplosionDisappearsAfterItsLastFrame) {
  EffectScene scene;
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.play(BYDO_EXPLOSION_LENGTH);

  EXPECT_TRUE(scene.shownEffects().empty());
}

/**
 * Given a player who reduced flashes
 * When a Bydo is killed
 * Then its explosion starts on the first frame of the version without flash
 */
TEST(EffectSpawner, WithFlashesReducedAnExplosionStartsWithoutItsFlash) {
  EffectScene scene;
  scene.effects().setFlashPreference(FlashPreference::Reduced);

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).frame,
            BYDO_EXPLOSION_WITHOUT_FLASH_FRAME);
}

/**
 * Given a spawner of effects
 * When a missile of a player appears
 * Then one effect shows the first frame of the player muzzle flash
 */
TEST(EffectSpawner, AMissileOfAPlayerFlashesAtItsMuzzle) {
  EffectScene scene;

  scene.effects().playAppearance(EntityType::PlayerMissile, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).frame, PLAYER_MUZZLE_FLASH_FRAME);
}

/**
 * Given a spawner of effects
 * When a missile of an enemy appears
 * Then one effect shows the first frame of the enemy muzzle flash
 */
TEST(EffectSpawner, AMissileOfAnEnemyFlashesAtItsMuzzle) {
  EffectScene scene;

  scene.effects().playAppearance(EntityType::EnemyMissile, WHERE_IT_DIED);

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.drawnSprite(shown.front()).frame, ENEMY_MUZZLE_FLASH_FRAME);
}

/**
 * Given a spawner of effects
 * When a ship and a Bydo appear
 * Then no effect is drawn
 */
TEST(EffectSpawner, AShipOrABydoAppearsWithoutEffect) {
  EffectScene scene;

  scene.effects().playAppearance(EntityType::Player, WHERE_IT_DIED);
  scene.effects().playAppearance(EntityType::Enemy, WHERE_IT_DIED);

  EXPECT_TRUE(scene.shownEffects().empty());
}

/**
 * Given a spawner of effects
 * When an entity of a type outside EntityType is killed
 * Then no effect is drawn
 */
TEST(EffectSpawner, AnExplosionOfAnUnknownTypePlaysNothing) {
  EffectScene scene;

  scene.effects().playExplosion(FIRST_VALUE_AFTER_THE_LAST_TYPE, WHERE_IT_DIED);

  EXPECT_TRUE(scene.shownEffects().empty());
}

/**
 * Given a spawner of effects
 * When an entity of a type outside EntityType appears
 * Then no effect is drawn
 */
TEST(EffectSpawner, AnAppearanceOfAnUnknownTypePlaysNothing) {
  EffectScene scene;

  scene.effects().playAppearance(FIRST_VALUE_AFTER_THE_LAST_TYPE,
                                 WHERE_IT_DIED);

  EXPECT_TRUE(scene.shownEffects().empty());
}

/**
 * Given a drawn entity
 * When it is hit
 * Then it is drawn with the flash tone
 */
TEST(EffectSpawner, AHitEntityFlashesWhite) {
  EffectScene scene;
  const Entity target = scene.target();

  scene.effects().flashHit(target);

  const HitFlash* flash = scene.flashOf(target);
  ASSERT_NE(flash, nullptr);
  EXPECT_EQ(flash->tone, SpriteTone::Flash);
}

/**
 * Given a drawn entity
 * When it is hit
 * Then it is flashed for 60 ms, the length the animation page shows
 */
TEST(EffectSpawner, AHitEntityFlashesForSixtyMilliseconds) {
  EffectScene scene;
  const Entity target = scene.target();

  scene.effects().flashHit(target);

  const HitFlash* flash = scene.flashOf(target);
  ASSERT_NE(flash, nullptr);
  EXPECT_EQ(flash->remaining, HIT_FLASH_LENGTH);
}

/**
 * Given a player who reduced flashes, and a drawn entity
 * When the entity is hit
 * Then it is drawn with the shade tone
 */
TEST(EffectSpawner, WithFlashesReducedAHitEntityIsShaded) {
  EffectScene scene;
  scene.effects().setFlashPreference(FlashPreference::Reduced);
  const Entity target = scene.target();

  scene.effects().flashHit(target);

  const HitFlash* flash = scene.flashOf(target);
  ASSERT_NE(flash, nullptr);
  EXPECT_EQ(flash->tone, SpriteTone::Shade);
}

/**
 * Given an entity destroyed before its hit arrives
 * When it is hit
 * Then nothing fails
 */
TEST(EffectSpawner, ADeadEntityIsNotFlashed) {
  EffectScene scene;
  const Entity target = scene.target();
  scene.components().destroy(target);

  EXPECT_NO_THROW(scene.effects().flashHit(target));
}

/**
 * Given an entity that has no Sprite
 * When it is hit
 * Then it gets no flash
 */
TEST(EffectSpawner, AnEntityWithoutSpriteIsNotFlashed) {
  EffectScene scene;
  const Entity unseen = scene.entities().create();

  scene.effects().flashHit(unseen);

  EXPECT_EQ(scene.flashOf(unseen), nullptr);
}
