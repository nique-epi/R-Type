#include "IntroCues.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <span>
#include <vector>
#include "IntroCast.hpp"
#include "IntroCastConstants.hpp"
#include "IntroCue.hpp"
#include "IntroFreeze.hpp"
#include "IntroMissile.hpp"
#include "IntroMobPaths.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "SoundConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The cues fixed by the beats of the intro. */
std::vector<IntroCue> fixedCues(std::span<const IntroFreeze> freezes) {
  return {IntroCue{.at = 0.0F, .sound = BYDO_GROWL_SOUND_ID},
          IntroCue{.at = INTRO_PREY_HIT,
                   .sound = SMALL_EXPLOSION_SOUND_ID,
                   .trauma = INTRO_PREY_TRAUMA},
          IntroCue{.at = INTRO_SWARM_CUT, .sound = ENTRY_WHOOSH_SOUND_ID},
          IntroCue{.at = INTRO_FIRST_KILL, .sound = HEAVY_IMPACT_SOUND_ID},
          IntroCue{.at = INTRO_REINFORCEMENTS, .sound = ENTRY_WHOOSH_SOUND_ID},
          IntroCue{.at = INTRO_SCENE_CHARGE, .sound = CHARGE_SOUND_ID},
          IntroCue{.at = INTRO_SCENE_RELEASE,
                   .sound = BEAM_SOUND_ID,
                   .trauma = INTRO_RELEASE_TRAUMA},
          IntroCue{.at = INTRO_SCENE_RELEASE + INTRO_BURST_DELAY,
                   .sound = BIG_EXPLOSION_SOUND_ID},
          IntroCue{.at = sceneTimeAt(INTRO_RELEASE + INTRO_AFTERSHOCK_DELAY,
                                     freezes),
                   .sound = {},
                   .trauma = INTRO_AFTERSHOCK_TRAUMA},
          IntroCue{.at = INTRO_SCENE_BOOST, .sound = BOOST_SOUND_ID},
          IntroCue{.at = sceneTimeAt(INTRO_SLAM + INTRO_SLAM_TIME, freezes),
                   .sound = LOGO_IMPACT_SOUND_ID}};
}

/** @brief Adds a flyby at the start of each close-up. */
void addFlybys(std::vector<IntroCue>& cues) {
  for (std::size_t pilot = 0; pilot < INTRO_HOMES.size(); ++pilot) {
    cues.push_back(IntroCue{.at = closeupStartOf(pilot) + INTRO_FLYBY_DELAY,
                            .sound = FLYBY_SOUND_ID});
  }
}

/** @brief Adds the shot of every missile, as it leaves. */
void addShots(std::vector<IntroCue>& cues, const IntroCast& cast) {
  for (const IntroMissile& missile : cast.missiles) {
    cues.push_back(IntroCue{
        .at = missile.start,
        .sound = missile.enemy ? ENEMY_SHOT_SOUND_ID : PLAYER_SHOT_SOUND_ID});
  }
}

/** @brief Adds the explosion and the shake of each Bydo of the helix, the
 * shake stronger for each later kill. */
void addHelixKills(std::vector<IntroCue>& cues) {
  for (std::size_t bydo = 0; bydo < INTRO_HELIX_SIZE; ++bydo) {
    const float share =
        static_cast<float>(bydo) / static_cast<float>(INTRO_HELIX_SIZE - 1);
    cues.push_back(IntroCue{
        .at = helixHitAt(bydo),
        .sound = SMALL_EXPLOSION_SOUND_ID,
        .trauma =
            std::lerp(INTRO_FIRST_KILL_TRAUMA, INTRO_LAST_KILL_TRAUMA, share)});
  }
}

}  // namespace

std::vector<IntroCue> introCues(const IntroCast& cast,
                                std::span<const IntroFreeze> freezes) {
  std::vector<IntroCue> cues = fixedCues(freezes);
  addFlybys(cues);
  addShots(cues, cast);
  addHelixKills(cues);
  std::ranges::stable_sort(cues, {}, &IntroCue::at);
  return cues;
}

}  // namespace rtype::client
