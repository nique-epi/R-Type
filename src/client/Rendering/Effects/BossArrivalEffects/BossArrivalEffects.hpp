#pragma once

#include <cstdint>
#include "EffectTools.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns How many steps of the boss walking in fall after @p previous and
 * at @p now at the latest, both since the Arriving status: one every
 * BOSS_JOLT_PERIOD from BOSS_ENTRY_START, BOSS_JOLT_COUNT in all. */
[[nodiscard]] int joltsBetween(engine::Duration previous, engine::Duration now);

/** @returns The trauma the camera is kept at, at that time since the Arriving
 * status: BOSS_RUMBLE_TRAUMA from BOSS_RUMBLE_START to BOSS_RUMBLE_END, none
 * otherwise. */
[[nodiscard]] float rumbleTraumaAt(engine::Duration sinceArrival);

/**
 * @brief The effects of the arrival of a boss on the playfield: the rumble of
 * the camera at the end of the WARNING screen, a jolt at each step of the
 * boss walking in, and at its roar a jolt and two shockwaves from its mouth.
 *
 * Every shake goes through the FlashGate, so REDUCE FLASHES leaves the camera
 * still; the shockwaves play either way, as the ring of a ship destroyed
 * does. It owns nothing: it plays with the tools of the EffectSpawner (see
 * EffectSpawner::tools()), which must outlive it. playRoar() adds
 * components, so it is never called during a forEach of the registry.
 */
class BossArrivalEffects {
 public:
  explicit BossArrivalEffects(EffectTools tools);

  /** @brief Shakes the camera as the time since the Arriving status goes
   * from @p previous to @p now: keeps its trauma at rumbleTraumaAt() at
   * least, and adds BOSS_JOLT_TRAUMA when a step falls in between. */
  void advance(engine::Duration previous, engine::Duration now) const;

  /** @brief Plays the roar of the boss of network id @p bossId:
   * BOSS_ROAR_TRAUMA, and the two shockwaves of the roar centered on its
   * mouth; no shockwave when the client has no such entity. */
  void playRoar(std::uint32_t bossId) const;

 private:
  EffectTools tools_;
};

}  // namespace rtype::client
