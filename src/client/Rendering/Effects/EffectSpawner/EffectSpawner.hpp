#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <span>
#include "CameraShake.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "FlashBudget.hpp"
#include "FlashPreference.hpp"
#include "GainAttribution.hpp"
#include "KillChain.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

struct Animation;
struct EffectAnimations;
struct HitFlash;
struct Nudge;

/**
 * @brief Plays the effects of a game on a ladder of intensity: a shot (T0), a
 * hit (T1), a Bydo destroyed (T2), every fifth kill of a chain (T3), a ship
 * destroyed (T4); and the arrival of a ship, its invulnerability, and the
 * figure of each gain.
 *
 * An effect of the sheet is an entity of its own, in the Effects layer,
 * centered on the position given, whose animation plays once; AnimationSystem
 * destroys it after its last frame. A destruction freezes its victim, never
 * the game: the first frame of its explosion stays a few frames longer, its
 * animation starting at minus that hold, and what follows (drops, debris,
 * shockwave) waits as long with a negative age (see PixelEffects).
 *
 * Every effect that flashes asks the FlashBudget first, and plays its version
 * without flash when refused. While the flash preference is
 * FlashPreference::Reduced, nothing flashes, shakes, recoils or vibrates: the
 * versions without flash play, a hit is shaded, the particles lose the first
 * color of their ramp, an invulnerable ship is dithered all along and
 * outlined, and an arrival shows no white. The preference is read when an
 * effect starts: an effect already playing keeps its version.
 *
 * advance() moves on what counts time: the game calls it every frame, before
 * applying the messages of the frame. Every other method but cameraOffset()
 * adds components, so none may be called during a forEach of the registry.
 * The registries must outlive the spawner.
 */
class EffectSpawner {
 public:
  /** @param seed Seed of the draws of the particles: std::random_device in
   * a game, a fixed value in a test. */
  EffectSpawner(engine::EntityRegistry& entities,
                engine::ComponentRegistry& components, std::uint32_t seed);

  /** @brief Sets which version of the effects starts from now on: the
   * standard one, or the one without flash. FlashPreference::Full until
   * set. */
  void setFlashPreference(FlashPreference preference);

  /** @brief Moves on the flash budget, the chain of kills, the kills and
   * gains waiting for each other, the camera shake, and the time since the
   * game started. */
  void advance(engine::Duration elapsed);

  /**
   * @brief Plays the explosion of an entity killed, centered where it died.
   *
   * A Bydo plays T2, or T3 when it is a big catch (see KillChain): its
   * explosion holds its first frame KILL_HOLD or BIG_CATCH_HOLD longer while
   * it vibrates, then its drops burst, and at T3 a shockwave opens and the
   * camera shakes; the figure of its gain shows once a score grows (see
   * noteScore()). A missile plays its impact and three sparks towards its
   * shooter. A ship plays its three fireballs alone: playShipDown() plays the
   * whole of T4. A value outside EntityType plays nothing.
   */
  void playExplosion(game::EntityType type, game::Position center);

  /**
   * @brief Plays T4 for a ship destroyed: its picture stays white, or
   * shaded, SHIP_DOWN_HOLD while it vibrates, then its three fireballs, its
   * debris and a shockwave in the colors of its pilot (see PilotShip), and the
   * camera shakes. Call it before the ship is destroyed. A ship without a
   * Position or a Sprite plays nothing.
   */
  void playShipDown(engine::Entity ship);

  /**
   * @brief Plays the appearance of an entity, centered where it appears: the
   * muzzle flash of a missile. The flash of a missile of a player follows the
   * nose of the nearest ship at its left, which recoils (T0); without such a
   * ship, it stays where the missile appeared. A ship or a Bydo appears
   * without effect: see playArrival() for a ship.
   */
  void playAppearance(game::EntityType type, game::Position center);

  /**
   * @brief Plays the arrival of a ship: hidden, then shown row by row over
   * MATERIALIZE_DURATION (see Reveal), behind four speed lines in the light
   * shade of its pilot. Asked before PILOT_ARRIVAL_DELAY into the game, it
   * waits until then. A ship without a Position is left alone.
   */
  void playArrival(engine::Entity ship);

  /**
   * @brief Plays T1 for an entity hit: white for HIT_FLASH_DURATION, or
   * shaded; then, when it has an EntityType and a Position, drawn a pixel
   * along the shot for KNOCKBACK_DURATION and three sparks towards its
   * shooter. A second hit starts it over. An entity that is not alive, or
   * has no Sprite, is left alone.
   */
  void playHit(engine::Entity entity);

  /** @brief Shows that a ship cannot be hit, or no longer can't: see
   * Invulnerability. Saying it twice changes nothing. */
  void setInvulnerable(engine::Entity ship, bool invulnerable);

  /** @brief Notes the score a world state gives the pilot of a color; a
   * score that grew shows as the figure of a gain where a Bydo died (see
   * GainAttribution). */
  void noteScore(int pilotColor, std::uint32_t score);

  /** @returns How many pixels of the canvas the sky and the entities are
   * drawn away from their place: the camera shake, (0, 0) at rest. */
  [[nodiscard]] sf::Vector2i cameraOffset() const;

  /** @returns The time advance() moved on since the spawner was built: the
   * time since the start of the game. */
  [[nodiscard]] engine::Duration sinceStart() const;

 private:
  /** @brief An effect of the sheet: its two versions, where it plays, how
   * much longer its first frame stays, and the pixels its flash takes for how
   * long. */
  struct SheetEffect {
    const EffectAnimations* animations{};
    game::Position center{};
    engine::Duration hold{};
    FlashBudget::Loan flash;
  };

  /** @brief What a ship destroyed leaves to its effects: where it stood, the
   * sprite it showed, and the color of its pilot when known. */
  struct DownedShip {
    game::Position center{};
    game::Sprite sprite{};
    std::optional<int> pilotColor;
  };

  /** @brief The ship a missile of a player came from, and where it stands. */
  struct Shooter {
    engine::Entity ship{};
    game::Position center{};
  };

  /** @brief What an entity hit is, and where it stands. */
  struct HitSpot {
    game::EntityType type{};
    game::Position center{};
  };

  void playKill(game::Position center);
  void playImpact(game::EntityType type, game::Position center);
  void playShot(game::Position center);

  /** @returns The entity of an effect of the sheet, or nothing for an effect
   * without frame. */
  std::optional<engine::Entity> spawn(const SheetEffect& effect);

  /** @returns The standard version of the effect when flashes are allowed
   * and the budget lends its pixels, else the version without flash. */
  const Animation& versionOf(const SheetEffect& effect);

  /** @returns A new entity showing the first frame of the animation, which
   * starts at minus the hold of the effect. */
  std::optional<engine::Entity> addSheetEntity(const Animation& animation,
                                               const SheetEffect& effect);

  /** @returns What a ship leaves, or nothing without Position or Sprite. */
  std::optional<DownedShip> downedShipOf(engine::Entity ship);

  /** @brief Leaves the picture of a ship destroyed, white or shaded, in the
   * Players layer, for SHIP_DOWN_HOLD. */
  void freeze(const DownedShip& ship);

  /** @brief Anchors the muzzle flash of a missile of a player that appears
   * at @p center to the nose of its shooter, which recoils; nothing without
   * a shooter. */
  void followShooter(engine::Entity muzzle, game::Position center);

  /** @returns What an entity hit is and where it stands, or nothing for an
   * entity without EntityType or Position. */
  std::optional<HitSpot> hitSpotOf(engine::Entity entity);

  /** @returns The nearest ship at the left of a missile of a player that
   * appears, or nothing. */
  std::optional<Shooter> shooterOf(game::Position missile);

  void flash(engine::Entity entity);

  /** @brief Draws an entity a few pixels away for a moment (see Nudge),
   * unless flashes are reduced. */
  void nudge(engine::Entity entity, const Nudge& push);

  /** @brief Bursts three sparks towards the shooter of a shot that touched
   * an entity of that type. */
  void burstSparks(game::EntityType type, game::Position center);

  void shake(float trauma);

  /** @returns Whether an effect flashes: flashes are allowed and the budget
   * lends the pixels of the loan, which it then holds. */
  [[nodiscard]] bool mayFlash(FlashBudget::Loan loan);

  /** @returns A hit flash lasting as long as the loan: white when it may
   * flash, else shaded. */
  [[nodiscard]] HitFlash hitFlashFor(FlashBudget::Loan loan);

  /** @brief Shows the figure of a gain once a kill and a gain wait for each
   * other. */
  void showGain();
  void addGainFigure(const GainAttribution::Figure& figure);

  /** @returns The ramp, or the ramp without its first color when flashes
   * are reduced. */
  [[nodiscard]] std::span<const sf::Color> rampOf(
      std::span<const sf::Color> ramp) const;

  [[nodiscard]] bool flashes() const;

  /** @returns The color of the pilot of a ship, or nothing for a ship
   * without PilotShip or with a color outside 0 to PLAYER_COLOR_COUNT - 1. */
  [[nodiscard]] std::optional<int> pilotColorOf(engine::Entity ship);

  engine::EntityRegistry* entities_;
  engine::ComponentRegistry* components_;
  FlashPreference flashes_ = FlashPreference::Full;
  FlashBudget flashBudget_;
  KillChain killChain_;
  GainAttribution gains_;
  CameraShake shake_;
  PixelEffects pixels_;
  engine::Duration sinceStart_{};
};

}  // namespace rtype::client
