#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "BackgroundColorConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectPreferences.hpp"
#include "EffectScene.hpp"
#include "EffectSpawner.hpp"
#include "EffectSpawnerFixtures.hpp"
#include "EffectSpawnerTestConstants.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Particle.hpp"
#include "PilotShadeConstants.hpp"
#include "PilotTestConstants.hpp"
#include "PlayerPaletteTestConstants.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "Sprite.hpp"
#include "SpriteTone.hpp"
#include "SpriteToneColorConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::BYDO_EXPLOSION_WITHOUT_FLASH_FRAME;
using rtype::client::Caption;
using rtype::client::DEUTERANOPIA_PILOT_SHADES;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectPreferences;
using rtype::client::ENEMY_MUZZLE_FLASH_FRAME;
using rtype::client::FLASH_COLOR;
using rtype::client::FlashPreference;
using rtype::client::HitFlash;
using rtype::client::IMPACT_FRAME;
using rtype::client::Invulnerability;
using rtype::client::Particle;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::SHIP_EXPLOSION_FRAME;
using rtype::client::Shockwave;
using rtype::client::SpeedLine;
using rtype::client::SpriteTone;
using rtype::client::STAR_GOLD;
using rtype::client::TextFont;
using rtype::client::testing::BLUISH_GREEN;
using rtype::client::testing::LIGHT_BLUISH_GREEN;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::EntityType;
using rtype::game::Layer;
using rtype::game::Position;

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then one effect shows the first frame of the Bydo explosion
 */
TEST(EffectSpawner, AKilledBydoBurstsIntoDrops) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            BYDO_EXPLOSION_FRAME);
}

/**
 * Given a spawner of effects
 * When a ship is killed
 * Then one effect shows the first frame of the chain of fireballs
 */
TEST(EffectSpawner, AKilledShipPlaysTheChainOfFireballs) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Player, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            SHIP_EXPLOSION_FRAME);
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

    const std::vector<Entity> shown = shownEffects(scene.components());
    ASSERT_EQ(shown.size(), 1U);
    EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
              IMPACT_FRAME);
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

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).assetId,
            EFFECT_SHEET_ID);
}

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then its explosion is drawn in the Effects layer, in front of the Bydos and
 * behind the ships
 */
TEST(EffectSpawner, AnEffectIsDrawnInTheEffectsLayer) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  EXPECT_EQ(shownEffects(scene.components()).size(), 1U);
}

/**
 * Given a spawner of effects
 * When a Bydo is killed at a position
 * Then its explosion is centered on that position
 */
TEST(EffectSpawner, AnEffectIsCenteredWhereTheEntityDied) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(positionOf(scene.components(), shown.front()).x, WHERE_IT_DIED.x);
  EXPECT_EQ(positionOf(scene.components(), shown.front()).y, WHERE_IT_DIED.y);
}

/**
 * Given a Bydo explosion just started
 * When one nanosecond less than 470 ms passes, its freeze of 50 ms and its
 * six frames of 70 ms
 * Then the explosion is still drawn
 */
TEST(EffectSpawner, AnExplosionStaysUntilItsLastFrameHasShown) {
  EffectScene scene;
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.play(KILL_HOLD_LENGTH + BYDO_EXPLOSION_LENGTH - ONE_TICK);

  EXPECT_EQ(shownEffects(scene.components()).size(), 1U);
}

/**
 * Given a Bydo explosion just started
 * When 470 ms pass, its freeze of 50 ms and its six frames of 70 ms as on the
 * animation page
 * Then no effect is left
 */
TEST(EffectSpawner, AnExplosionDisappearsAfterItsLastFrame) {
  EffectScene scene;
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.play(KILL_HOLD_LENGTH + BYDO_EXPLOSION_LENGTH);

  EXPECT_TRUE(shownEffects(scene.components()).empty());
}

/**
 * Given a player who reduced flashes
 * When a Bydo is killed
 * Then its explosion starts on the first frame of the version without flash
 */
TEST(EffectSpawner, WithFlashesReducedAnExplosionStartsWithoutItsFlash) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced});

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
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

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            PLAYER_MUZZLE_FLASH_FRAME);
}

/**
 * Given a spawner of effects
 * When a missile of an enemy appears
 * Then one effect shows the first frame of the enemy muzzle flash
 */
TEST(EffectSpawner, AMissileOfAnEnemyFlashesAtItsMuzzle) {
  EffectScene scene;

  scene.effects().playAppearance(EntityType::EnemyMissile, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(drawnSprite(scene.components(), shown.front()).frame,
            ENEMY_MUZZLE_FLASH_FRAME);
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

  EXPECT_TRUE(shownEffects(scene.components()).empty());
}

/**
 * Given a spawner of effects
 * When an entity of a type outside EntityType is killed
 * Then no effect is drawn
 */
TEST(EffectSpawner, AnExplosionOfAnUnknownTypePlaysNothing) {
  EffectScene scene;

  scene.effects().playExplosion(FIRST_VALUE_AFTER_THE_LAST_TYPE, WHERE_IT_DIED);

  EXPECT_TRUE(shownEffects(scene.components()).empty());
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

  EXPECT_TRUE(shownEffects(scene.components()).empty());
}

/**
 * Given a drawn entity
 * When it is hit
 * Then it is drawn with the flash tone
 */
TEST(EffectSpawner, AHitEntityFlashesWhite) {
  EffectScene scene;
  const Entity target = scene.target();

  scene.effects().playHit(target);

  const HitFlash* flash = flashOf(scene.components(), target);
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

  scene.effects().playHit(target);

  const HitFlash* flash = flashOf(scene.components(), target);
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
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced});
  const Entity target = scene.target();

  scene.effects().playHit(target);

  const HitFlash* flash = flashOf(scene.components(), target);
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

  EXPECT_NO_THROW(scene.effects().playHit(target));
}

/**
 * Given an entity that has no Sprite
 * When it is hit
 * Then it gets no flash
 */
TEST(EffectSpawner, AnEntityWithoutSpriteIsNotFlashed) {
  EffectScene scene;
  const Entity unseen = scene.entities().create();

  scene.effects().playHit(unseen);

  EXPECT_EQ(flashOf(scene.components(), unseen), nullptr);
}

/**
 * Given a spawner of effects
 * When a Bydo is killed
 * Then its explosion holds its first frame 50 ms longer, and 8 drops wait as
 * long
 */
TEST(EffectSpawner, AKilledBydoFreezesThenBurstsIntoEightDrops) {
  EffectScene scene;

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(framesHeldFor(scene.components(), shown.front()), KILL_HOLD_LENGTH);
  EXPECT_EQ(count<Particle>(scene.components()), KILL_DROP_COUNT);
  EXPECT_EQ(count<Shockwave>(scene.components()), 0U);
}

/**
 * Given four Bydos killed 100 ms apart
 * When a fifth is killed 100 ms later
 * Then it is a big catch: held 83 ms, 16 drops, and a shockwave
 */
TEST(EffectSpawner, TheFifthKillOfAChainIsABigCatch) {
  EffectScene scene;
  for (int kill = 1; kill < KILLS_PER_BIG_CATCH; ++kill) {
    scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);
    scene.advance(CLOSE_KILLS,
                  EffectPreferences{.flashes = FlashPreference::Full});
  }
  const std::size_t particlesBefore = count<Particle>(scene.components());

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  EXPECT_EQ(count<Particle>(scene.components()) - particlesBefore,
            BIG_CATCH_DROP_COUNT);
  EXPECT_EQ(count<Shockwave>(scene.components()), 1U);
  EXPECT_EQ(framesHeldFor(scene.components(),
                          shownEffects(scene.components()).back()),
            BIG_CATCH_HOLD_LENGTH);
}

/**
 * Given 13 Bydos killed in the same frame, 13 x 275 = 3575 pixels of white
 * When a fourteenth is killed
 * Then its explosion plays without its flash: the white on screen stays
 * under 3600 pixels
 */
TEST(EffectSpawner, AnExplosionPastTheFlashBudgetPlaysWithoutFlash) {
  EffectScene scene;
  for (int kill = 0; kill < BYDO_FLASHES_WITHIN_BUDGET; ++kill) {
    scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);
  }

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(),
            static_cast<std::size_t>(BYDO_FLASHES_WITHIN_BUDGET) + 1);
  EXPECT_EQ(drawnSprite(scene.components(), shown[shown.size() - 2]).frame,
            BYDO_EXPLOSION_FRAME);
  EXPECT_EQ(drawnSprite(scene.components(), shown.back()).frame,
            BYDO_EXPLOSION_WITHOUT_FLASH_FRAME);
}

/**
 * Given a ship with a pilot, a Position and a Sprite
 * When it is destroyed
 * Then its picture stays white for 100 ms, and its three fireballs wait as
 * long
 */
TEST(EffectSpawner, ADestroyedShipFreezesWhiteBeforeItsFireballs) {
  EffectScene scene;
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().playShipDown(ship);

  const std::optional<Entity> picture = frozenPicture(scene.components());
  ASSERT_TRUE(picture.has_value());
  const Entity frozen = picture.value_or(Entity{});
  EXPECT_EQ(toneOf(scene.components(), frozen), SpriteTone::Flash);
  EXPECT_EQ(drawnSprite(scene.components(), frozen).layer, Layer::Players);
  EXPECT_EQ(framesHeldFor(scene.components(),
                          shownEffects(scene.components()).back()),
            SHIP_DOWN_HOLD_LENGTH);
}

/**
 * Given a ship destroyed
 * When 100 ms pass
 * Then its white picture is gone
 */
TEST(EffectSpawner, TheWhitePictureOfAShipGoesAfterItsFreeze) {
  EffectScene scene;
  scene.effects().playShipDown(scene.shipOfPilot(SECOND_PILOT_COLOR));

  scene.advance(SHIP_DOWN_HOLD_LENGTH,
                EffectPreferences{.flashes = FlashPreference::Full});

  EXPECT_FALSE(frozenPicture(scene.components()).has_value());
}

/**
 * Given the ship of the second pilot destroyed
 * When its debris are read
 * Then 24 pieces, going through the pink of the second pilot
 */
TEST(EffectSpawner, TheDebrisOfAShipAreInTheColorsOfItsPilot) {
  EffectScene scene;

  scene.effects().playShipDown(scene.shipOfPilot(SECOND_PILOT_COLOR));

  EXPECT_EQ(count<Particle>(scene.components()), SHIP_DEBRIS_COUNT);
  EXPECT_TRUE(
      everyParticleGoesThrough(scene.components(), PINK_OF_THE_SECOND_PILOT));
}

/**
 * Given a player who reduced flashes
 * When a ship is destroyed
 * Then its picture is shaded, the camera does not move, and no particle
 * starts in white
 */
TEST(EffectSpawner, WithFlashesReducedAShipGoesDownWithoutFlashNorShake) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced});

  scene.effects().playShipDown(scene.shipOfPilot(SECOND_PILOT_COLOR));
  scene.advance(ONE_FRAME,
                EffectPreferences{.flashes = FlashPreference::Reduced});

  const std::optional<Entity> picture = frozenPicture(scene.components());
  ASSERT_TRUE(picture.has_value());
  const Entity frozen = picture.value_or(Entity{});
  EXPECT_EQ(toneOf(scene.components(), frozen), SpriteTone::Shade);
  EXPECT_EQ(scene.effects().cameraOffset(), sf::Vector2i());
  EXPECT_FALSE(anyParticleStartsWith(scene.components(), FLASH_COLOR));
}

/**
 * Given a ship at the left of where a missile of a player appears
 * When the missile appears
 * Then its muzzle flash is anchored to the ship, and the ship recoils
 */
TEST(EffectSpawner, AShotFlashFollowsTheShipThatFired) {
  EffectScene scene;
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().playAppearance(EntityType::PlayerMissile, AHEAD_OF_THE_SHIP);

  const std::vector<Entity> shown = shownEffects(scene.components());
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(anchorOf(scene.components(), shown.front()), ship);
  EXPECT_TRUE(isNudged(scene.components(), ship));
}

/**
 * Given a Bydo with a Position and an EntityType
 * When it is hit
 * Then it flashes, is drawn a pixel along the shot, and three sparks fly
 */
TEST(EffectSpawner, AHitBydoFlashesRecoilsAndSparks) {
  EffectScene scene;
  const Entity bydo = scene.bydoAt(WHERE_IT_DIED);

  scene.effects().playHit(bydo);

  EXPECT_EQ(toneOf(scene.components(), bydo), SpriteTone::Flash);
  EXPECT_TRUE(isNudged(scene.components(), bydo));
  EXPECT_EQ(count<Particle>(scene.components()), SPARK_COUNT);
}

/**
 * Given a ship arriving at the start of the game
 * When its arrival plays
 * Then it is hidden for 500 ms, then revealed, behind four speed lines
 */
TEST(EffectSpawner, AShipArrivingAtTheStartWaitsForThePilots) {
  EffectScene scene;
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().playArrival(ship);

  EXPECT_EQ(revealAgeOf(scene.components(), ship), -PILOT_ARRIVAL_WAIT);
  EXPECT_EQ(count<SpeedLine>(scene.components()), SPEED_LINE_COUNT);
}

/**
 * Given a player who reduced flashes
 * When a ship becomes invulnerable
 * Then it is dithered all along, never blinking
 */
TEST(EffectSpawner, WithFlashesReducedAnInvulnerableShipIsSteady) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced});
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().setInvulnerable(ship, true);

  EXPECT_TRUE(isSteadilyInvulnerable(scene.components(), ship));
}

/**
 * Given the second pilot at 1000 points and a Bydo killed
 * When its score reaches 1100
 * Then "P2 +100" shows, small, in the light of that pilot, in the Popups layer
 */
TEST(EffectSpawner, TheGainOfAKillShowsAsAFigure) {
  EffectScene scene;
  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE);
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE + DRIFTER_POINTS);

  const std::optional<Caption> figure = caption(scene.components());
  ASSERT_TRUE(figure.has_value());
  const Caption shown = figure.value_or(Caption{});
  EXPECT_EQ(shown.text, "P2 +100");
  EXPECT_EQ(shown.style.color, PINK_LIGHT_OF_THE_SECOND_PILOT);
  EXPECT_EQ(shown.style.font, TextFont::Small);
  EXPECT_EQ(shown.layer, Layer::Popups);
}

/**
 * Given the palette for deuteranopia
 * When the ship of the third pilot is destroyed
 * Then every particle of its debris goes through the bluish green of the
 * palette
 */
TEST(EffectSpawner, TheDebrisOfAShipFollowThePalette) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Full,
                                          .shades = DEUTERANOPIA_PILOT_SHADES});

  scene.effects().playShipDown(scene.shipOfPilot(THIRD_PILOT_COLOR));

  EXPECT_TRUE(everyParticleGoesThrough(scene.components(), BLUISH_GREEN));
}

/**
 * Given a player who reduced flashes and chose the palette for deuteranopia
 * When the ship of the third pilot becomes invulnerable
 * Then its invulnerability is outlined in the light bluish green of the
 * palette
 */
TEST(EffectSpawner, ASteadyInvulnerabilityIsOutlinedInThePalette) {
  EffectScene scene;
  scene.effects().advance(
      Duration::zero(), EffectPreferences{.flashes = FlashPreference::Reduced,
                                          .shades = DEUTERANOPIA_PILOT_SHADES});
  const Entity ship = scene.shipOfPilot(THIRD_PILOT_COLOR);

  scene.effects().setInvulnerable(ship, true);

  const Invulnerability* invulnerability =
      scene.components().get<Invulnerability>(ship);
  ASSERT_NE(invulnerability, nullptr);
  EXPECT_EQ(invulnerability->outline, LIGHT_BLUISH_GREEN);
}

/**
 * Given the first and the second pilots who both gained 100 points
 * When a Bydo is killed
 * Then the figure shows "+200" in gold: no pilot gained alone, so none is
 * named
 */
TEST(EffectSpawner, AGainOfSeveralPilotsNamesNoPilot) {
  EffectScene scene;
  scene.effects().noteScore(FIRST_PILOT_COLOR, SCORE_BEFORE);
  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE);
  scene.effects().noteScore(FIRST_PILOT_COLOR, SCORE_BEFORE + DRIFTER_POINTS);
  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE + DRIFTER_POINTS);

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  const std::optional<Caption> figure = caption(scene.components());
  ASSERT_TRUE(figure.has_value());
  const Caption shown = figure.value_or(Caption{});
  EXPECT_EQ(shown.text, "+200");
  EXPECT_EQ(shown.style.color, STAR_GOLD);
}

/**
 * Given the sound cues shown and a Bydo
 * When a missile appears where that Bydo fires
 * Then "!" shows in the light orange of the enemy missile, the middle of its
 * line 72 units above the center of the Bydo
 */
TEST(EffectSpawner, AShotCueShowsOverTheBydoThatFired) {
  EffectScene scene;
  scene.advance(ONE_FRAME, EffectPreferences{.soundCues = true});
  scene.bydoAt(FIRING_BYDO);

  scene.effects().playAppearance(EntityType::EnemyMissile,
                                 MISSILE_OF_THE_FIRING_BYDO);

  const std::optional<Entity> cue = captioned(scene.components());
  ASSERT_TRUE(cue.has_value());
  const Caption shown = caption(scene.components()).value_or(Caption{});
  const Position place = positionOf(scene.components(), cue.value_or(Entity{}));
  EXPECT_EQ(shown.text, "!");
  EXPECT_EQ(shown.style.color, LIGHT_ORANGE_OF_THE_ENEMY_MISSILE);
  EXPECT_EQ(place.x, ABOVE_THE_FIRING_BYDO.x);
  EXPECT_EQ(place.y, ABOVE_THE_FIRING_BYDO.y);
}

/**
 * Given the sound cues shown and a Bydo that just fired
 * When its cue is read
 * Then it is anchored to that Bydo, so it follows it
 */
TEST(EffectSpawner, AShotCueFollowsItsBydo) {
  EffectScene scene;
  scene.advance(ONE_FRAME, EffectPreferences{.soundCues = true});
  const Entity bydo = scene.bydoAt(FIRING_BYDO);

  scene.effects().playAppearance(EntityType::EnemyMissile,
                                 MISSILE_OF_THE_FIRING_BYDO);

  const std::optional<Entity> cue = captioned(scene.components());
  ASSERT_TRUE(cue.has_value());
  EXPECT_EQ(anchorOf(scene.components(), cue.value_or(Entity{})), bydo);
}

/**
 * Given the sound cues shown and the cue of a shot
 * When half a second passes
 * Then the cue is gone
 */
TEST(EffectSpawner, AShotCueGoesAfterHalfASecond) {
  EffectScene scene;
  scene.advance(ONE_FRAME, EffectPreferences{.soundCues = true});
  scene.bydoAt(FIRING_BYDO);
  scene.effects().playAppearance(EntityType::EnemyMissile,
                                 MISSILE_OF_THE_FIRING_BYDO);

  scene.advance(SHOT_CUE_LIFE_LENGTH, EffectPreferences{});

  EXPECT_EQ(count<Caption>(scene.components()), 0U);
}

/**
 * Given the sound cues shown and a ship of the second pilot
 * When it goes down
 * Then "P2 HIT" shows in the light of that pilot, the middle of its line 56
 * units above the center of the ship
 */
TEST(EffectSpawner, AHitCueNamesThePilotOverItsShip) {
  EffectScene scene;
  scene.advance(ONE_FRAME, EffectPreferences{.soundCues = true});
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().playShipDown(ship);

  const std::optional<Entity> cue = captioned(scene.components());
  ASSERT_TRUE(cue.has_value());
  const Caption shown = caption(scene.components()).value_or(Caption{});
  const Position place = positionOf(scene.components(), cue.value_or(Entity{}));
  EXPECT_EQ(shown.text, "P2 HIT");
  EXPECT_EQ(shown.style.color, PINK_LIGHT_OF_THE_SECOND_PILOT);
  EXPECT_EQ(place.x, ABOVE_THE_SHIP.x);
  EXPECT_EQ(place.y, ABOVE_THE_SHIP.y);
}

/**
 * Given the sound cues not shown, a Bydo and a ship
 * When the Bydo fires and the ship goes down
 * Then no cue shows
 */
TEST(EffectSpawner, NoCueShowsWhileTheSoundCuesAreOff) {
  EffectScene scene;
  scene.advance(ONE_FRAME, EffectPreferences{.flashes = FlashPreference::Full});
  scene.bydoAt(FIRING_BYDO);
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().playAppearance(EntityType::EnemyMissile,
                                 MISSILE_OF_THE_FIRING_BYDO);
  scene.effects().playShipDown(ship);

  EXPECT_EQ(count<Caption>(scene.components()), 0U);
}
