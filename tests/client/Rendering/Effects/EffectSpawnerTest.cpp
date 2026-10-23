#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "Anchor.hpp"
#include "AnimationSystem.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectLengths.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "FlashPreference.hpp"
#include "HitFlash.hpp"
#include "HitFlashSystem.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "Nudge.hpp"
#include "Particle.hpp"
#include "PilotShip.hpp"
#include "PilotTestConstants.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "ScreenTestConstants.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteEffectSystem.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "SpriteToneColorConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

using rtype::client::Anchor;
using rtype::client::AnimationSystem;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::BYDO_EXPLOSION_WITHOUT_FLASH_FRAME;
using rtype::client::Caption;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectSpawner;
using rtype::client::ENEMY_MUZZLE_FLASH_FRAME;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::FLASH_COLOR;
using rtype::client::FlashPreference;
using rtype::client::HitFlash;
using rtype::client::HitFlashSystem;
using rtype::client::IMPACT_FRAME;
using rtype::client::Invulnerability;
using rtype::client::Lifetime;
using rtype::client::Nudge;
using rtype::client::Particle;
using rtype::client::PilotShip;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::client::Reveal;
using rtype::client::SHIP_EXPLOSION_FRAME;
using rtype::client::Shockwave;
using rtype::client::SpeedLine;
using rtype::client::SpriteAnimation;
using rtype::client::SpriteEffectSystem;
using rtype::client::SpriteTone;
using rtype::client::TextFont;
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
constexpr Position SHIP_CENTER{.x = 400.0F, .y = 360.0F};
constexpr Position AHEAD_OF_THE_SHIP{.x = 460.0F, .y = 360.0F};
constexpr Duration CLOSE_KILLS = std::chrono::milliseconds(100);
constexpr int BYDO_FLASHES_WITHIN_BUDGET = 13;
constexpr std::uint32_t SCORE_BEFORE = 1000;
constexpr std::uint32_t DRIFTER_POINTS = 100;
constexpr std::size_t SPEED_LINE_COUNT = 4;

/** @brief Registries with a spawner of effects, and the systems that play
 * them. */
class EffectScene {
 public:
  EffectSpawner& effects() { return effects_; }

  void play(Duration elapsed) { animations_.update(components_, elapsed); }

  /** @brief Moves the spawner and the systems of the sprites on, as a frame
   * of the game screen does. */
  void advance(Duration elapsed, FlashPreference preference) {
    effects_.advance(elapsed, preference);
    animations_.update(components_, elapsed);
    hitFlashes_.update(components_, elapsed);
    spriteEffects_.update(components_, elapsed);
  }

  /** @returns A ship of the pilot of that color, as the replicator makes it,
   * at SHIP_CENTER. */
  Entity shipOfPilot(int pilotColor) {
    const Entity ship = entities_.create();
    components_.add(ship, SHIP_CENTER);
    components_.add(ship, EntityType::Player);
    components_.add(ship, Sprite{.assetId = ENTITY_SHEET_ID,
                                 .frame = PLAYER_SHIP_FRAME,
                                 .layer = Layer::Players});
    components_.add(ship, PilotShip{.color = pilotColor});
    return ship;
  }

  /** @returns A Bydo at that position, as the replicator makes it. */
  Entity bydoAt(Position position) {
    const Entity bydo = entities_.create();
    components_.add(bydo, position);
    components_.add(bydo, EntityType::Enemy);
    components_.add(bydo, Sprite{.assetId = "bydo",
                                 .frame = TARGET_FRAME,
                                 .layer = Layer::Enemies});
    return bydo;
  }

  /** @returns The white picture a ship destroyed leaves, or nothing. */
  std::optional<Entity> frozenPicture() {
    std::optional<Entity> found;
    components_.forEach<Lifetime, Sprite>(
        [&found](Entity entity, const Lifetime&, const Sprite&) {
          found = entity;
        });
    return found;
  }

  /** @returns How many entities carry a component of that type and a
   * Position. */
  template <typename Component>
  [[nodiscard]] std::size_t count() {
    std::size_t found = 0;
    components_.forEach<Component, Position>(
        [&found](Entity, const Component&, const Position&) { ++found; });
    return found;
  }

  /** @returns How much longer the first frame of an effect stays. */
  [[nodiscard]] Duration framesHeldFor(Entity effect) {
    const SpriteAnimation* animation = components_.get<SpriteAnimation>(effect);
    return animation != nullptr ? -animation->elapsed : Duration::zero();
  }

  /** @returns The tone an entity is drawn with by its hit flash, Normal
   * without one. */
  [[nodiscard]] SpriteTone toneOf(Entity entity) {
    const HitFlash* flash = components_.get<HitFlash>(entity);
    return flash != nullptr ? flash->tone : SpriteTone::Normal;
  }

  /** @returns The entity an effect follows, or nothing. */
  [[nodiscard]] std::optional<Entity> anchorOf(Entity effect) {
    const Anchor* anchor = components_.get<Anchor>(effect);
    return anchor != nullptr ? std::optional<Entity>(anchor->entity)
                             : std::nullopt;
  }

  [[nodiscard]] bool isNudged(Entity entity) {
    return components_.has<Nudge>(entity);
  }

  /** @returns The age of the reveal of a ship arriving, or zero without
   * one. */
  [[nodiscard]] Duration revealAgeOf(Entity ship) {
    const Reveal* reveal = components_.get<Reveal>(ship);
    return reveal != nullptr ? reveal->age : Duration::zero();
  }

  [[nodiscard]] bool isSteadilyInvulnerable(Entity ship) {
    const Invulnerability* invulnerability =
        components_.get<Invulnerability>(ship);
    return invulnerability != nullptr && invulnerability->steady;
  }

  /** @returns Whether every particle goes through that color as it ages. */
  [[nodiscard]] bool everyParticleGoesThrough(sf::Color color) {
    bool every = true;
    components_.forEach<Particle, Position>(
        [&every, color](Entity, const Particle& particle, const Position&) {
          every = every && std::ranges::find(particle.colors, color) !=
                               particle.colors.end();
        });
    return every;
  }

  /** @returns Whether a particle starts in that color. */
  [[nodiscard]] bool anyParticleStartsWith(sf::Color color) {
    bool any = false;
    components_.forEach<Particle, Position>(
        [&any, color](Entity, const Particle& particle, const Position&) {
          any = any ||
                (!particle.colors.empty() && particle.colors.front() == color);
        });
    return any;
  }

  /** @returns The first figure of a gain, or nothing. */
  [[nodiscard]] std::optional<Caption> caption() {
    std::optional<Caption> found;
    components_.forEach<Caption, Position>(
        [&found](Entity, const Caption& figure, const Position&) {
          if (!found.has_value()) {
            found = figure;
          }
        });
    return found;
  }

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
                                   .layer = Layer::Enemies});
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
  EffectSpawner effects_{entities_, components_, EFFECT_TEST_SEED};
  AnimationSystem animations_;
  HitFlashSystem hitFlashes_;
  SpriteEffectSystem spriteEffects_;
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
 * Then its explosion is drawn in the Effects layer, in front of the Bydos and
 * behind the ships
 */
TEST(EffectSpawner, AnEffectIsDrawnInTheEffectsLayer) {
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
 * When one nanosecond less than 470 ms passes, its freeze of 50 ms and its
 * six frames of 70 ms
 * Then the explosion is still drawn
 */
TEST(EffectSpawner, AnExplosionStaysUntilItsLastFrameHasShown) {
  EffectScene scene;
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.play(KILL_HOLD_LENGTH + BYDO_EXPLOSION_LENGTH - ONE_TICK);

  EXPECT_EQ(scene.shownEffects().size(), 1U);
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

  EXPECT_TRUE(scene.shownEffects().empty());
}

/**
 * Given a player who reduced flashes
 * When a Bydo is killed
 * Then its explosion starts on the first frame of the version without flash
 */
TEST(EffectSpawner, WithFlashesReducedAnExplosionStartsWithoutItsFlash) {
  EffectScene scene;
  scene.effects().advance(Duration::zero(), FlashPreference::Reduced);

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

  scene.effects().playHit(target);

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

  scene.effects().playHit(target);

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
  scene.effects().advance(Duration::zero(), FlashPreference::Reduced);
  const Entity target = scene.target();

  scene.effects().playHit(target);

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

  EXPECT_EQ(scene.flashOf(unseen), nullptr);
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

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.framesHeldFor(shown.front()), KILL_HOLD_LENGTH);
  EXPECT_EQ(scene.count<Particle>(), KILL_DROP_COUNT);
  EXPECT_EQ(scene.count<Shockwave>(), 0U);
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
    scene.advance(CLOSE_KILLS, FlashPreference::Full);
  }
  const std::size_t particlesBefore = scene.count<Particle>();

  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  EXPECT_EQ(scene.count<Particle>() - particlesBefore, BIG_CATCH_DROP_COUNT);
  EXPECT_EQ(scene.count<Shockwave>(), 1U);
  EXPECT_EQ(scene.framesHeldFor(scene.shownEffects().back()),
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

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(),
            static_cast<std::size_t>(BYDO_FLASHES_WITHIN_BUDGET) + 1);
  EXPECT_EQ(scene.drawnSprite(shown[shown.size() - 2]).frame,
            BYDO_EXPLOSION_FRAME);
  EXPECT_EQ(scene.drawnSprite(shown.back()).frame,
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

  const std::optional<Entity> picture = scene.frozenPicture();
  ASSERT_TRUE(picture.has_value());
  const Entity frozen = picture.value_or(Entity{});
  EXPECT_EQ(scene.toneOf(frozen), SpriteTone::Flash);
  EXPECT_EQ(scene.drawnSprite(frozen).layer, Layer::Players);
  EXPECT_EQ(scene.framesHeldFor(scene.shownEffects().back()),
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

  scene.advance(SHIP_DOWN_HOLD_LENGTH, FlashPreference::Full);

  EXPECT_FALSE(scene.frozenPicture().has_value());
}

/**
 * Given the ship of the second pilot destroyed
 * When its debris are read
 * Then 24 pieces, going through the pink of the second pilot
 */
TEST(EffectSpawner, TheDebrisOfAShipAreInTheColorsOfItsPilot) {
  EffectScene scene;

  scene.effects().playShipDown(scene.shipOfPilot(SECOND_PILOT_COLOR));

  EXPECT_EQ(scene.count<Particle>(), SHIP_DEBRIS_COUNT);
  EXPECT_TRUE(scene.everyParticleGoesThrough(PINK_OF_THE_SECOND_PILOT));
}

/**
 * Given a player who reduced flashes
 * When a ship is destroyed
 * Then its picture is shaded, the camera does not move, and no particle
 * starts in white
 */
TEST(EffectSpawner, WithFlashesReducedAShipGoesDownWithoutFlashNorShake) {
  EffectScene scene;
  scene.effects().advance(Duration::zero(), FlashPreference::Reduced);

  scene.effects().playShipDown(scene.shipOfPilot(SECOND_PILOT_COLOR));
  scene.advance(ONE_FRAME, FlashPreference::Reduced);

  const std::optional<Entity> picture = scene.frozenPicture();
  ASSERT_TRUE(picture.has_value());
  const Entity frozen = picture.value_or(Entity{});
  EXPECT_EQ(scene.toneOf(frozen), SpriteTone::Shade);
  EXPECT_EQ(scene.effects().cameraOffset(), sf::Vector2i());
  EXPECT_FALSE(scene.anyParticleStartsWith(FLASH_COLOR));
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

  const std::vector<Entity> shown = scene.shownEffects();
  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(scene.anchorOf(shown.front()), ship);
  EXPECT_TRUE(scene.isNudged(ship));
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

  EXPECT_EQ(scene.toneOf(bydo), SpriteTone::Flash);
  EXPECT_TRUE(scene.isNudged(bydo));
  EXPECT_EQ(scene.count<Particle>(), SPARK_COUNT);
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

  EXPECT_EQ(scene.revealAgeOf(ship), -PILOT_ARRIVAL_WAIT);
  EXPECT_EQ(scene.count<SpeedLine>(), SPEED_LINE_COUNT);
}

/**
 * Given a player who reduced flashes
 * When a ship becomes invulnerable
 * Then it is dithered all along, never blinking
 */
TEST(EffectSpawner, WithFlashesReducedAnInvulnerableShipIsSteady) {
  EffectScene scene;
  scene.effects().advance(Duration::zero(), FlashPreference::Reduced);
  const Entity ship = scene.shipOfPilot(SECOND_PILOT_COLOR);

  scene.effects().setInvulnerable(ship, true);

  EXPECT_TRUE(scene.isSteadilyInvulnerable(ship));
}

/**
 * Given a pilot at 1000 points and a Bydo killed
 * When its score reaches 1100
 * Then "+100" shows, small, in the light of that pilot, in the Popups layer
 */
TEST(EffectSpawner, TheGainOfAKillShowsAsAFigure) {
  EffectScene scene;
  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE);
  scene.effects().playExplosion(EntityType::Enemy, WHERE_IT_DIED);

  scene.effects().noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE + DRIFTER_POINTS);

  const std::optional<Caption> figure = scene.caption();
  ASSERT_TRUE(figure.has_value());
  const Caption shown = figure.value_or(Caption{});
  EXPECT_EQ(shown.text, "+100");
  EXPECT_EQ(shown.style.color, PINK_LIGHT_OF_THE_SECOND_PILOT);
  EXPECT_EQ(shown.style.font, TextFont::Small);
  EXPECT_EQ(shown.layer, Layer::Popups);
}
