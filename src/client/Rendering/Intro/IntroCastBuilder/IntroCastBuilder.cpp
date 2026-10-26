#include "IntroCastBuilder.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <vector>
#include "BackgroundConstants.hpp"
#include "IntroBlast.hpp"
#include "IntroBurst.hpp"
#include "IntroCast.hpp"
#include "IntroCastConstants.hpp"
#include "IntroMissile.hpp"
#include "IntroMobPaths.hpp"
#include "IntroRing.hpp"
#include "IntroShipPaths.hpp"
#include "IntroSpark.hpp"
#include "IntroSparkSpread.hpp"
#include "IntroTimelineConstants.hpp"
#include "RandomRange.hpp"
#include "RenderingConstants.hpp"
#include "SquadronConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Where and when a burst plays, and how many drops it throws. */
struct BurstSpot {
  sf::Vector2f center;
  float start{};
  std::size_t drops{};
};

/** @returns Where the muzzle of the ship of @p pilot is at @p scene, or
 * where it is at @p fallback when the ship is not on screen at @p scene. */
sf::Vector2f muzzleAt(std::size_t pilot, float scene, float fallback) {
  const std::optional<sf::Vector2f> place = shipPlaceAt(pilot, scene);
  const sf::Vector2f shown =
      place.value_or(shipPlaceAt(pilot, fallback).value_or(sf::Vector2f()));
  return shown + SQUADRON_MUZZLE;
}

/** @returns The missile @p pilot fires so that it hits @p target at @p hit,
 * the moment of the shot refined INTRO_AIM_PASSES times from where the ship
 * is when it fires. */
IntroMissile aimedMissile(std::size_t pilot, sf::Vector2f target, float hit) {
  float start = hit - INTRO_AIM_FIRST_GUESS;
  for (int pass = 0; pass < INTRO_AIM_PASSES; ++pass) {
    const sf::Vector2f flight = target - muzzleAt(pilot, start, hit);
    start = hit - (flight.length() / INTRO_PLAYER_MISSILE_SPEED);
  }
  return IntroMissile{.from = muzzleAt(pilot, start, hit),
                      .to = target,
                      .start = start,
                      .end = hit};
}

/** @returns The missile an enemy fires from @p origin to @p destination at
 * @p start. */
IntroMissile enemyShot(sf::Vector2f origin, sf::Vector2f destination,
                       float start) {
  return IntroMissile{.from = origin,
                      .to = destination,
                      .start = start,
                      .end = start + ((destination - origin).length() /
                                      INTRO_ENEMY_MISSILE_SPEED),
                      .enemy = true};
}

/** @returns The pilot whose ship, ready to fire, is closest in height to
 * @p target at @p scene; the first pilot when none is ready. */
std::size_t closestPilotTo(sf::Vector2f target, float scene) {
  std::size_t closest = 0;
  float closestGap = std::numeric_limits<float>::infinity();
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    const std::optional<sf::Vector2f> place = shipPlaceAt(pilot, scene);
    if (scene < INTRO_SQUADRON_READY || !place.has_value()) {
      continue;
    }
    const float gap = std::abs(place->y + INTRO_SHIP_HALF.y - target.y);
    if (gap < closestGap) {
      closestGap = gap;
      closest = pilot;
    }
  }
  return closest;
}

/** @returns @p count sparks or drops, each heading anywhere, drawn as
 * @p spread says. */
std::vector<IntroSpark> sparksOf(const IntroSparkSpread& spread,
                                 std::size_t count, std::mt19937& random) {
  std::vector<IntroSpark> sparks;
  sparks.reserve(count);
  for (std::size_t spark = 0; spark < count; ++spark) {
    const float heading = randomBetween(random, 0.0F, FULL_TURN);
    const float speed = randomBetween(random, spread.slowest, spread.fastest);
    sparks.push_back(IntroSpark{
        .velocity = {std::cos(heading) * speed, std::sin(heading) * speed},
        .life = randomBetween(random, spread.shortestLife, spread.longestLife),
        .delay = randomBetween(random, 0.0F, spread.latestStart),
        .size = randomChance(random, spread.largeChance) ? INTRO_LARGE_SPARK
                                                         : INTRO_SMALL_SPARK});
  }
  return sparks;
}

/** @returns The explosions of the big burst, scattered over
 * INTRO_BIG_BLAST_SPREAD, one after the other. */
std::vector<IntroBlast> bigBlasts(std::mt19937& random) {
  std::vector<IntroBlast> blasts;
  blasts.reserve(INTRO_BIG_BLASTS);
  for (std::size_t blast = 0; blast < INTRO_BIG_BLASTS; ++blast) {
    blasts.push_back(IntroBlast{
        .offset = randomPointBetween(random, -INTRO_BIG_BLAST_SPREAD * HALF,
                                     INTRO_BIG_BLAST_SPREAD * HALF),
        .delay = static_cast<float>(blast) * INTRO_BIG_BLAST_STAGGER,
        .fireball = blast % INTRO_BIG_BLAST_PERIOD != 0});
  }
  return blasts;
}

/** @returns A small burst at @p spot: one burst of a Bydo, its sparks and its
 * drops. */
IntroBurst smallBurst(const BurstSpot& spot, std::mt19937& random) {
  return IntroBurst{
      .center = spot.center,
      .start = spot.start,
      .blasts = {IntroBlast{}},
      .sparks = sparksOf(INTRO_SMALL_SPARKS, INTRO_SMALL_SPARK_COUNT, random),
      .drops = sparksOf(INTRO_SMALL_DROPS, spot.drops, random)};
}

/** @returns The big burst at @p spot. */
IntroBurst bigBurst(const BurstSpot& spot, std::mt19937& random) {
  return IntroBurst{
      .center = spot.center,
      .start = spot.start,
      .blasts = bigBlasts(random),
      .sparks = sparksOf(INTRO_BIG_SPARKS, INTRO_BIG_SPARK_COUNT, random),
      .drops = sparksOf(INTRO_BIG_DROPS, spot.drops, random)};
}

/** @returns @p missile, leaving no earlier than INTRO_SQUADRON_READY, from
 * where the ship of @p pilot is then: a missile aimed from far would
 * otherwise leave before its ship is on screen. It still hits when it was
 * aimed to, faster. */
IntroMissile fromReadyShip(IntroMissile missile, std::size_t pilot) {
  if (missile.start >= INTRO_SQUADRON_READY) {
    return missile;
  }
  missile.start = INTRO_SQUADRON_READY;
  missile.from = muzzleAt(pilot, INTRO_SQUADRON_READY, missile.end);
  return missile;
}

/** @returns @p missile swerving for kill @p kill: further for each later
 * kill, on each side in turn. */
IntroMissile curvedFor(IntroMissile missile, std::size_t kill) {
  const float side = kill % INTRO_SIDES == 0 ? 1.0F : -1.0F;
  missile.curve = side * (INTRO_KILL_CURVE +
                          (static_cast<float>(kill) * INTRO_KILL_CURVE_STEP));
  return missile;
}

/** @brief Adds the shot of the Bydo in the dark, and the missile and the
 * burst of the prey. */
void addOpening(IntroCast& cast, std::mt19937& random) {
  cast.missiles.push_back(enemyShot(INTRO_COLD_OPEN_SHOT_FROM,
                                    INTRO_COLD_OPEN_SHOT_TO,
                                    INTRO_COLD_OPEN_SHOT_START));
  const sf::Vector2f prey = preyCenterAt(INTRO_PREY_HIT);
  cast.missiles.push_back(aimedMissile(INTRO_PREY_PILOT, prey, INTRO_PREY_HIT));
  cast.bursts.push_back(smallBurst(
      BurstSpot{.center = prey, .start = INTRO_PREY_HIT, .drops = 0}, random));
}

/** @brief Adds, for each Bydo of the helix, its missile, its burst and its
 * ring. */
void addHelixKills(IntroCast& cast, std::mt19937& random) {
  for (std::size_t bydo = 0; bydo < INTRO_HELIX_SIZE; ++bydo) {
    const float hit = helixHitAt(bydo);
    const sf::Vector2f target = helixCenterAt(bydo, hit);
    const std::size_t pilot = closestPilotTo(target, hit - INTRO_AIM_LOOKBACK);
    cast.missiles.push_back(curvedFor(
        fromReadyShip(aimedMissile(pilot, target, hit), pilot), bydo));
    cast.bursts.push_back(smallBurst(
        BurstSpot{.center = target, .start = hit, .drops = INTRO_HELIX_DROPS},
        random));
    cast.rings.push_back(IntroRing{.center = target,
                                   .start = hit,
                                   .radius = INTRO_KILL_RING_RADIUS,
                                   .life = INTRO_KILL_RING_LIFE});
  }
}

/** @brief Adds the two shots of the helix at the swap, off the left edge. */
void addHelixShots(IntroCast& cast) {
  for (std::size_t shot = 0; shot < INTRO_SHOOTING_BYDOS.size(); ++shot) {
    const sf::Vector2f from =
        helixCenterAt(INTRO_SHOOTING_BYDOS.at(shot), INTRO_SWAP);
    const sf::Vector2f destination(INTRO_HELIX_SHOT_TARGET_X,
                                   from.y + INTRO_HELIX_SHOT_DROPS.at(shot));
    cast.missiles.push_back(enemyShot(from, destination, INTRO_SWAP));
  }
}

/** @brief Adds the bursts of the newcomers, then the big burst and the big
 * ring of the ring. */
void addFinale(IntroCast& cast, std::mt19937& random) {
  for (std::size_t newcomer = 0; newcomer < INTRO_RING_SIZE; ++newcomer) {
    const float hit = ringHitAt(newcomer);
    cast.bursts.push_back(
        smallBurst(BurstSpot{.center = newcomerCenterAt(newcomer, hit),
                             .start = hit,
                             .drops = INTRO_RING_DROPS},
                   random));
  }
  const float burst = INTRO_SCENE_RELEASE + INTRO_BURST_DELAY;
  cast.bursts.push_back(bigBurst(BurstSpot{.center = INTRO_RING_CENTER,
                                           .start = burst,
                                           .drops = INTRO_BIG_DROP_COUNT},
                                 random));
  cast.rings.push_back(IntroRing{.center = INTRO_RING_CENTER,
                                 .start = burst,
                                 .radius = INTRO_BIG_RING_RADIUS,
                                 .life = INTRO_BIG_RING_LIFE});
}

}  // namespace

IntroCast buildIntroCast(std::uint32_t seed) {
  std::mt19937 random(seed);
  IntroCast cast;
  addOpening(cast, random);
  addHelixKills(cast, random);
  addHelixShots(cast);
  addFinale(cast, random);
  return cast;
}

}  // namespace rtype::client
