#pragma once

#include "SoundConstants.hpp"

namespace rtype::client {

/** @brief The sounds of the arrival of a boss: a step of the boss walking
 * in, and its roar; both reuse a sound of the client until the team picks
 * its own. Its alarm is BOSS_ALARM_SOUND_ID. */
constexpr const char* BOSS_STEP_SOUND_ID = HEAVY_IMPACT_SOUND_ID;
constexpr const char* BOSS_ROAR_SOUND_ID = BYDO_GROWL_SOUND_ID;

}  // namespace rtype::client
