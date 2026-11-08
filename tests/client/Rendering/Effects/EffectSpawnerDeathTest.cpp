#include <gtest/gtest.h>
#include <vector>
#include "EffectConstants.hpp"
#include "EffectPreferences.hpp"
#include "EffectScene.hpp"
#include "EffectSpawner.hpp"
#include "EffectSpawnerFixtures.hpp"
#include "EffectSpawnerTestConstants.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "Particle.hpp"
#include "TimeConstants.hpp"

using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectPreferences;
using rtype::client::EntityAppearance;
using rtype::client::FlashPreference;
using rtype::client::Particle;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::EntityType;

/**
 * Given an enemy whose sheet draws its own death
 * When it is killed
 * Then its explosion is the first frame of that death, from its own sheet
 */
TEST(EffectSpawnerDeath, AnEnemyKilledPlaysTheDeathOfItsSheet) {
  EffectScene scene;
  const EntityAppearance appearance = appearanceWithADeath();

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED, &appearance);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            MOB_DEATH_FRAME);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).assetId,
            MOB_SHEET_ID);
}

/**
 * Given a player who reduced flashes, and an enemy whose sheet draws its own
 * death
 * When it is killed
 * Then its death starts on the first frame of its version without flash
 */
TEST(EffectSpawnerDeath, WithFlashesReducedTheDeathStartsWithoutItsFlash) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced});
  const EntityAppearance appearance = appearanceWithADeath();

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED, &appearance);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            MOB_DEATH_WITHOUT_FLASH_FRAME);
}

/**
 * Given an enemy whose sheet draws no death
 * When it is killed
 * Then it plays the Bydo explosion, from the effect sheet
 */
TEST(EffectSpawnerDeath, AnEnemyWithoutADeathPlaysTheBydoExplosion) {
  EffectScene scene;
  EntityAppearance appearance = appearanceWithADeath();
  appearance.deathFrames.clear();
  appearance.deathFramesWithoutFlash.clear();

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED, &appearance);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            BYDO_EXPLOSION_FRAME);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).assetId,
            EFFECT_SHEET_ID);
}

/**
 * Given an enemy whose sheet draws its own death, and a Bydo
 * When each is killed in a scene of its own
 * Then both burst into as many drops: only the explosion changes
 */
TEST(EffectSpawnerDeath, AnEnemyWithItsOwnDeathKeepsTheDropsOfAKill) {
  EffectScene withDeath;
  EffectScene bydo;
  const EntityAppearance appearance = appearanceWithADeath();

  withDeath.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED,
                                    &appearance);
  bydo.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  ASSERT_GT(count<Particle>(bydo.components()), 0U);
  EXPECT_EQ(count<Particle>(withDeath.components()),
            count<Particle>(bydo.components()));
}
