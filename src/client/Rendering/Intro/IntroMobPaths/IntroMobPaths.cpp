#include "IntroMobPaths.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <optional>
#include "BackgroundConstants.hpp"
#include "Easing.hpp"
#include "IntroCastConstants.hpp"
#include "IntroMob.hpp"
#include "IntroMobKind.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"

namespace rtype::client {

namespace {

/** @returns When newcomer @p index starts its glide. */
float arrivalOf(std::size_t index) {
  return INTRO_REINFORCEMENTS +
         (static_cast<float>(index) * INTRO_NEWCOMER_STAGGER);
}

/** @returns The place of slot @p slot of the ring: it turns faster and faster
 * during the charge, and shrinks. */
sf::Vector2f ringSlotAt(std::size_t slot, float scene) {
  const float charged = std::max(0.0F, scene - INTRO_SCENE_CHARGE);
  const float spin = (INTRO_RING_SPIN * (scene - INTRO_REINFORCEMENTS)) +
                     (INTRO_RING_CHARGE_SPIN * charged * charged);
  const float radius =
      std::lerp(INTRO_RING_RADIUS, INTRO_RING_CHARGED_RADIUS,
                progressThrough(scene, INTRO_SCENE_CHARGE, INTRO_BEAT));
  const float angle = (static_cast<float>(slot) * FULL_TURN /
                       static_cast<float>(INTRO_RING_SIZE)) +
                      spin;
  return INTRO_RING_CENTER +
         sf::Vector2f(std::cos(angle) * radius,
                      std::sin(angle) * radius * INTRO_RING_FLATTEN);
}

/** @returns The center of a Bydo crossing a close-up, @p since seconds after
 * the close-up started, on row @p row. */
sf::Vector2f crossingCenterAt(float since, float row) {
  return {INTRO_CROSSER_START_X - (INTRO_CROSSER_SPEED * since), row};
}

/** @returns Whether @p scene falls in the span of @p length from
 * @p start. */
bool isWithin(float scene, float start, float length) {
  return scene >= start && scene < start + length;
}

/** @returns @p mob while it is @p shown, else nothing. */
std::optional<IntroMob> shownWhile(bool shown, IntroMob mob) {
  return shown ? std::optional<IntroMob>(mob) : std::nullopt;
}

}  // namespace

sf::Vector2f helixCenterAt(std::size_t index, float scene) {
  const auto strand = static_cast<float>(index % INTRO_HELIX_STRANDS);
  const std::size_t rank = index / INTRO_HELIX_STRANDS;
  const auto order = static_cast<float>(rank);
  const float since = scene - (INTRO_SWARM_CUT - INTRO_HELIX_LEAD +
                               (order * INTRO_HELIX_STAGGER));
  const float phase =
      strand * FULL_TURN / static_cast<float>(INTRO_HELIX_STRANDS);
  return {
      INTRO_HELIX_START_X - (INTRO_HELIX_SPEED * since),
      INTRO_HELIX_MIDDLE_Y + (INTRO_HELIX_AMPLITUDE *
                              std::sin((INTRO_HELIX_WAVE * since) + phase))};
}

sf::Vector2f newcomerCenterAt(std::size_t index, float scene) {
  const auto order = static_cast<float>(index);
  const float side = index % INTRO_SIDES == 0 ? 1.0F : -1.0F;
  const float rest = 1.0F - easeOutCubic(progressThrough(
                                scene, arrivalOf(index), INTRO_NEWCOMER_GLIDE));
  const sf::Vector2f reach(
      INTRO_NEWCOMER_REACH + (order * INTRO_NEWCOMER_REACH_STEP),
      side * (INTRO_NEWCOMER_SWOOP + (order * INTRO_NEWCOMER_SWOOP_STEP)));
  return ringSlotAt(index, scene) + (reach * rest);
}

sf::Vector2f preyCenterAt(float scene) {
  return INTRO_PREY_START -
         sf::Vector2f(INTRO_PREY_SPEED * (scene - INTRO_PREY_APPEARS), 0.0F);
}

float helixHitAt(std::size_t index) {
  return sceneTimeAt(INTRO_FIRST_KILL + INTRO_KILL_GAPS.at(index),
                     INTRO_FREEZES);
}

float ringHitAt(std::size_t index) {
  return INTRO_SCENE_RELEASE + INTRO_RING_HIT_DELAY +
         (static_cast<float>(index) * INTRO_RING_HIT_STAGGER);
}

std::array<std::optional<IntroMob>, INTRO_OPENING_MOBS> openingMobsAt(
    float scene) {
  const float crossing = closeupStartOf(INTRO_CROSSED_PILOT);
  const float bonusCrossing = closeupStartOf(INTRO_BONUS_CROSSED_PILOT);
  return {
      shownWhile(
          scene < INTRO_BEAT,
          IntroMob{.center = INTRO_COLD_OPEN_FOCUS -
                             sf::Vector2f(INTRO_COLD_OPEN_DRIFT * scene, 0.0F),
                   .kind = IntroMobKind::Bydo}),
      shownWhile(isWithin(scene, crossing, INTRO_BEAT),
                 IntroMob{.center = crossingCenterAt(scene - crossing,
                                                     INTRO_CROSSER_ROW),
                          .kind = IntroMobKind::Bydo}),
      shownWhile(
          scene >= INTRO_PREY_APPEARS && scene < INTRO_PREY_HIT,
          IntroMob{.center = preyCenterAt(scene), .kind = IntroMobKind::Bydo}),
      shownWhile(isWithin(scene, bonusCrossing, INTRO_BEAT),
                 IntroMob{.center = crossingCenterAt(scene - bonusCrossing,
                                                     INTRO_BONUS_CROSSER_ROW),
                          .kind = IntroMobKind::BonusBydo})};
}

std::optional<IntroMob> helixMobAt(std::size_t index, float scene) {
  const float hit = helixHitAt(index);
  return shownWhile(scene >= INTRO_SWARM_CUT && scene < hit + INTRO_DEATH_DELAY,
                    IntroMob{.center = helixCenterAt(index, scene),
                             .kind = index >= INTRO_HELIX_FIRST_BONUS
                                         ? IntroMobKind::BonusBydo
                                         : IntroMobKind::Bydo,
                             .flashed = scene >= hit - INTRO_HIT_FLASH_LEAD});
}

std::optional<IntroMob> newcomerMobAt(std::size_t index, float scene) {
  const float hit = ringHitAt(index);
  return shownWhile(
      scene >= arrivalOf(index) && scene < hit + INTRO_DEATH_DELAY,
      IntroMob{.center = newcomerCenterAt(index, scene),
               .kind = index >= INTRO_RING_FIRST_BONUS ? IntroMobKind::BonusBydo
                                                       : IntroMobKind::Bydo,
               .flashed = scene >= hit - INTRO_HIT_FLASH_LEAD});
}

}  // namespace rtype::client
