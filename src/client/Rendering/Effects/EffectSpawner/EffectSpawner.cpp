#include "EffectSpawner.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include "Anchor.hpp"
#include "Animation.hpp"
#include "BackgroundColors.hpp"
#include "CameraShakeConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectColors.hpp"
#include "EffectConstants.hpp"
#include "EffectTiers.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "FlashBudget.hpp"
#include "FlashPreference.hpp"
#include "GainAttribution.hpp"
#include "GroundEffectConstants.hpp"
#include "HitFlash.hpp"
#include "InterfaceColors.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "MissionStartConstants.hpp"
#include "Nudge.hpp"
#include "PilotShip.hpp"
#include "Popup.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "Reveal.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTone.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The direction the shooter of a shot that touched an entity of
 * that type stands in: the players shoot to the right, the enemies to the
 * left. */
float towardsShooterOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Enemy:
    case game::EntityType::PlayerMissile:
      return TOWARDS_LEFT;
    case game::EntityType::Player:
    case game::EntityType::EnemyMissile:
      break;
  }
  return TOWARDS_RIGHT;
}

/** @returns The knockback of an entity of that type hit: drawn a pixel
 * along the shot, a ship to the left, anything else to the right, for
 * KNOCKBACK_DURATION. */
Nudge knockbackOf(game::EntityType type) {
  return Nudge{.amount = type == game::EntityType::Player ? -KNOCKBACK_OFFSET
                                                          : KNOCKBACK_OFFSET,
               .flipEvery = engine::Duration::zero(),
               .elapsed = engine::Duration::zero(),
               .duration = KNOCKBACK_DURATION};
}

/** @returns The pixels the silhouette of an entity covers: a ship's, or a
 * Bydo's, the largest, for anything else. */
int silhouetteAreaOf(const game::EntityType* type) {
  return type != nullptr && *type == game::EntityType::Player
             ? SHIP_SILHOUETTE_AREA
             : BYDO_SILHOUETTE_AREA;
}

/** @returns The pixels a ship explosion flashes, from now until its first
 * SHIP_EXPLOSION_FLASH_FRAMES frames passed after its hold. */
FlashBudget::Loan shipExplosionFlash(engine::Duration hold) {
  return FlashBudget::Loan{.area = SHIP_EXPLOSION_FLASH_AREA,
                           .remaining = hold + (SHIP_EXPLOSION_FRAME_TIME *
                                                SHIP_EXPLOSION_FLASH_FRAMES)};
}

std::span<const sf::Color> debrisRampOf(std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_DEBRIS_RAMP;
  }
  return PILOT_DEBRIS_RAMPS[static_cast<std::size_t>(*pilotColor)];
}

std::span<const sf::Color> ringRampOf(std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_RING_RAMP;
  }
  return debrisRampOf(pilotColor).subspan(1);
}

sf::Color lightOf(std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_LIGHT;
  }
  return PLAYER_COLORS[static_cast<std::size_t>(*pilotColor)].light;
}

/** @returns How the figure of a gain is written: large for a big catch, in
 * the light shade of the only pilot who gained, else STAR_GOLD, outlined in
 * INK so the sky never touches a letter. */
TextStyle figureStyleOf(const GainAttribution::Figure& figure) {
  return TextStyle{.font = figure.big ? TextFont::Regular : TextFont::Small,
                   .color = figure.pilotColor.has_value()
                                ? lightOf(figure.pilotColor)
                                : STAR_GOLD,
                   .outlined = true,
                   .alignment = TextAlignment::Center};
}

/** @returns The square of the distance between two points. */
float squaredDistance(game::Position from, game::Position target) {
  return (sf::Vector2f(target.x, target.y) - sf::Vector2f(from.x, from.y))
      .lengthSquared();
}

/** @returns The vibration of a victim frozen for @p hold: one pixel to each
 * side in turn, over the first half of the freeze. */
Nudge vibrationOf(engine::Duration hold) {
  return Nudge{
      .amount = VIBRATION_OFFSET,
      .flipEvery = VIBRATION_STEP,
      .elapsed = engine::Duration::zero(),
      .duration = std::chrono::duration_cast<engine::Duration>(hold * HALF)};
}

}  // namespace

EffectSpawner::EffectSpawner(engine::EntityRegistry& entities,
                             engine::ComponentRegistry& components,
                             std::uint32_t seed)
    : entities_(&entities),
      components_(&components),
      pixels_(entities, components, seed) {}

void EffectSpawner::setFlashPreference(FlashPreference preference) {
  flashes_ = preference;
}

void EffectSpawner::advance(engine::Duration elapsed) {
  flashBudget_.advance(elapsed);
  killChain_.advance(elapsed);
  gains_.advance(elapsed);
  shake_.advance(elapsed);
  sinceStart_ += elapsed;
}

void EffectSpawner::playExplosion(game::EntityType type,
                                  game::Position center) {
  switch (type) {
    case game::EntityType::Enemy:
      playKill(center);
      return;
    case game::EntityType::Player:
      spawn(SheetEffect{.animations = &SHIP_EXPLOSION,
                        .center = center,
                        .hold = engine::Duration::zero(),
                        .flash = shipExplosionFlash(engine::Duration::zero())});
      return;
    case game::EntityType::PlayerMissile:
    case game::EntityType::EnemyMissile:
      playImpact(type, center);
      return;
  }
}

void EffectSpawner::playShipDown(engine::Entity ship) {
  const std::optional<DownedShip> downed = downedShipOf(ship);
  if (!downed.has_value()) {
    return;
  }
  freeze(*downed);
  spawn(SheetEffect{.animations = &SHIP_EXPLOSION,
                    .center = downed->center,
                    .hold = SHIP_DOWN_HOLD,
                    .flash = shipExplosionFlash(SHIP_DOWN_HOLD)});
  pixels_.burst({.shape = SHIP_DEBRIS,
                 .center = downed->center,
                 .aim = TOWARDS_RIGHT,
                 .colors = rampOf(debrisRampOf(downed->pilotColor)),
                 .delay = SHIP_DOWN_HOLD});
  pixels_.wave({.shape = SHIP_DOWN_RING,
                .center = downed->center,
                .colors = ringRampOf(downed->pilotColor),
                .delay = SHIP_DOWN_HOLD});
  shake(SHIP_DOWN_TRAUMA);
}

void EffectSpawner::playAppearance(game::EntityType type,
                                   game::Position center) {
  switch (type) {
    case game::EntityType::PlayerMissile:
      playShot(center);
      return;
    case game::EntityType::EnemyMissile:
      spawn(SheetEffect{.animations = &ENEMY_MUZZLE_FLASH,
                        .center = center,
                        .hold = engine::Duration::zero(),
                        .flash = {.area = MUZZLE_FLASH_AREA,
                                  .remaining = MUZZLE_FLASH_FRAME_TIME}});
      return;
    case game::EntityType::Player:
    case game::EntityType::Enemy:
      return;
  }
}

void EffectSpawner::playArrival(engine::Entity ship) {
  const game::Position* found = components_->get<game::Position>(ship);
  if (found == nullptr) {
    return;
  }
  const game::Position center = *found;
  const engine::Duration delay =
      std::max(engine::Duration::zero(), PILOT_ARRIVAL_DELAY - sinceStart_);
  components_->add(ship, Reveal{.age = -delay,
                                .duration = MATERIALIZE_DURATION,
                                .leadingLine = flashes()});
  pixels_.speedLines({.shipCenter = center,
                      .light = lightOf(pilotColorOf(ship)),
                      .flashes = flashes(),
                      .delay = delay,
                      .life = MATERIALIZE_DURATION});
}

void EffectSpawner::playHit(engine::Entity entity) {
  if (!components_->has<game::Sprite>(entity)) {
    return;
  }
  flash(entity);
  if (const std::optional<HitSpot> spot = hitSpotOf(entity); spot.has_value()) {
    nudge(entity, knockbackOf(spot->type));
    burstSparks(spot->type, spot->center);
  }
}

void EffectSpawner::setInvulnerable(engine::Entity ship, bool invulnerable) {
  if (!invulnerable) {
    components_->remove<Invulnerability>(ship);
    return;
  }
  if (components_->has<Invulnerability>(ship)) {
    return;
  }
  components_->add(ship, Invulnerability{.elapsed = engine::Duration::zero(),
                                         .steady = !flashes()});
}

void EffectSpawner::noteScore(int pilotColor, std::uint32_t score) {
  gains_.noteScore(pilotColor, score);
  showGain();
}

sf::Vector2i EffectSpawner::cameraOffset() const { return shake_.offset(); }

engine::Duration EffectSpawner::sinceStart() const { return sinceStart_; }

void EffectSpawner::playKill(game::Position center) {
  const bool bigCatch = killChain_.noteKill();
  const KillTier& tier = bigCatch ? BIG_CATCH_TIER : KILL_TIER;
  const std::optional<engine::Entity> explosion = spawn(SheetEffect{
      .animations = &BYDO_EXPLOSION,
      .center = center,
      .hold = tier.hold,
      .flash = {.area = BYDO_EXPLOSION_FLASH_AREA,
                .remaining = tier.hold + BYDO_EXPLOSION_FRAME_TIME}});
  if (explosion.has_value()) {
    nudge(*explosion, vibrationOf(tier.hold));
  }
  pixels_.burst({.shape = tier.drops,
                 .center = center,
                 .aim = TOWARDS_RIGHT,
                 .colors = rampOf(BYDO_RAMP),
                 .delay = tier.hold});
  if (tier.ring.has_value()) {
    pixels_.wave({.shape = *tier.ring,
                  .center = center,
                  .colors = rampOf(BIG_CATCH_RING_RAMP),
                  .delay = tier.hold});
  }
  shake(tier.trauma);
  gains_.noteKill(center, bigCatch);
  showGain();
}

void EffectSpawner::playImpact(game::EntityType type, game::Position center) {
  spawn(SheetEffect{
      .animations = &IMPACT,
      .center = center,
      .hold = engine::Duration::zero(),
      .flash = {.area = IMPACT_FLASH_AREA,
                .remaining = IMPACT_FRAME_TIME * IMPACT_FLASH_FRAMES}});
  burstSparks(type, center);
}

void EffectSpawner::playShot(game::Position center) {
  const std::optional<engine::Entity> muzzle =
      spawn(SheetEffect{.animations = &PLAYER_MUZZLE_FLASH,
                        .center = center,
                        .hold = engine::Duration::zero(),
                        .flash = {.area = MUZZLE_FLASH_AREA,
                                  .remaining = MUZZLE_FLASH_FRAME_TIME}});
  if (muzzle.has_value()) {
    followShooter(*muzzle, center);
  }
}

void EffectSpawner::followShooter(engine::Entity muzzle,
                                  game::Position center) {
  const std::optional<Shooter> shooter = shooterOf(center);
  if (!shooter.has_value()) {
    return;
  }
  components_->add(muzzle, Anchor{.entity = shooter->ship,
                                  .offset = {center.x - shooter->center.x,
                                             center.y - shooter->center.y}});
  nudge(shooter->ship, Nudge{.amount = RECOIL_OFFSET,
                             .flipEvery = engine::Duration::zero(),
                             .elapsed = engine::Duration::zero(),
                             .duration = RECOIL_DURATION});
}

std::optional<engine::Entity> EffectSpawner::spawn(const SheetEffect& effect) {
  return addSheetEntity(versionOf(effect), effect);
}

const Animation& EffectSpawner::versionOf(const SheetEffect& effect) {
  return mayFlash(effect.flash) ? effect.animations->standard
                                : effect.animations->withoutFlash;
}

std::optional<engine::Entity> EffectSpawner::addSheetEntity(
    const Animation& animation, const SheetEffect& effect) {
  const std::optional<game::SpriteFrame> firstFrame =
      animation.frameAt(engine::Duration::zero());
  if (!firstFrame.has_value()) {
    return std::nullopt;
  }
  const engine::Entity entity = entities_->create();
  components_->add(entity, effect.center);
  components_->add(entity, game::Sprite{.assetId = EFFECT_SHEET_ID,
                                        .frame = *firstFrame,
                                        .layer = game::Layer::Effects});
  components_->add(
      entity, SpriteAnimation{.animation = animation, .elapsed = -effect.hold});
  return entity;
}

std::optional<EffectSpawner::DownedShip> EffectSpawner::downedShipOf(
    engine::Entity ship) {
  const game::Position* position = components_->get<game::Position>(ship);
  const game::Sprite* sprite = components_->get<game::Sprite>(ship);
  if (position == nullptr || sprite == nullptr) {
    return std::nullopt;
  }
  return DownedShip{
      .center = *position, .sprite = *sprite, .pilotColor = pilotColorOf(ship)};
}

void EffectSpawner::freeze(const DownedShip& ship) {
  const engine::Entity picture = entities_->create();
  components_->add(picture, ship.center);
  components_->add(picture, game::Sprite{.assetId = ship.sprite.assetId,
                                         .frame = ship.sprite.frame,
                                         .layer = game::Layer::Players});
  components_->add(picture, hitFlashFor({.area = SHIP_SILHOUETTE_AREA,
                                         .remaining = SHIP_DOWN_HOLD}));
  components_->add(picture, Lifetime{.remaining = SHIP_DOWN_HOLD});
  nudge(picture, vibrationOf(SHIP_DOWN_HOLD));
}

std::optional<EffectSpawner::HitSpot> EffectSpawner::hitSpotOf(
    engine::Entity entity) {
  const game::EntityType* type = components_->get<game::EntityType>(entity);
  const game::Position* position = components_->get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return std::nullopt;
  }
  return HitSpot{.type = *type, .center = *position};
}

std::optional<EffectSpawner::Shooter> EffectSpawner::shooterOf(
    game::Position missile) {
  std::optional<Shooter> nearest;
  float nearestDistance = std::numeric_limits<float>::max();
  components_->forEach<game::EntityType, game::Position>(
      [&nearest, &nearestDistance, missile](engine::Entity entity,
                                            game::EntityType& type,
                                            game::Position& position) {
        const float distance = squaredDistance(position, missile);
        if (type == game::EntityType::Player && position.x <= missile.x &&
            distance < nearestDistance) {
          nearest = Shooter{.ship = entity, .center = position};
          nearestDistance = distance;
        }
      });
  return nearest;
}

void EffectSpawner::flash(engine::Entity entity) {
  const int area = silhouetteAreaOf(components_->get<game::EntityType>(entity));
  components_->add(
      entity, hitFlashFor({.area = area, .remaining = HIT_FLASH_DURATION}));
}

void EffectSpawner::nudge(engine::Entity entity, const Nudge& push) {
  if (flashes()) {
    components_->add(entity, push);
  }
}

void EffectSpawner::burstSparks(game::EntityType type, game::Position center) {
  pixels_.burst({.shape = IMPACT_SPARKS,
                 .center = center,
                 .aim = towardsShooterOf(type),
                 .colors = rampOf(SPARK_RAMP),
                 .delay = engine::Duration::zero()});
}

bool EffectSpawner::mayFlash(FlashBudget::Loan loan) {
  return flashes() && flashBudget_.reserve(loan);
}

HitFlash EffectSpawner::hitFlashFor(FlashBudget::Loan loan) {
  return HitFlash{
      .tone = mayFlash(loan) ? SpriteTone::Flash : SpriteTone::Shade,
      .remaining = loan.remaining};
}

void EffectSpawner::shake(float trauma) {
  if (flashes()) {
    shake_.addTrauma(trauma);
  }
}

void EffectSpawner::showGain() {
  if (const std::optional<GainAttribution::Figure> figure = gains_.takeFigure();
      figure.has_value()) {
    addGainFigure(*figure);
  }
}

void EffectSpawner::addGainFigure(const GainAttribution::Figure& figure) {
  const engine::Entity entity = entities_->create();
  components_->add(entity, figure.place);
  components_->add(entity,
                   Caption{.text = GAIN_PREFIX + std::to_string(figure.points),
                           .style = figureStyleOf(figure),
                           .layer = game::Layer::Popups,
                           .visible = true});
  components_->add(entity, Popup{.originY = figure.place.y,
                                 .age = engine::Duration::zero()});
}

std::span<const sf::Color> EffectSpawner::rampOf(
    std::span<const sf::Color> ramp) const {
  return flashes() || ramp.empty() ? ramp : ramp.subspan(1);
}

bool EffectSpawner::flashes() const {
  return flashes_ == FlashPreference::Full;
}

std::optional<int> EffectSpawner::pilotColorOf(engine::Entity ship) {
  const PilotShip* pilot = components_->get<PilotShip>(ship);
  if (pilot == nullptr || !isPilotColor(pilot->color)) {
    return std::nullopt;
  }
  return pilot->color;
}

}  // namespace rtype::client
