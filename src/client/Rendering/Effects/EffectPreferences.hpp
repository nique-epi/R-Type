#pragma once

#include "FlashPreference.hpp"
#include "PilotShadeConstants.hpp"
#include "PilotShades.hpp"

namespace rtype::client {

/** @brief What the player asked of the effects: whether they may flash, the
 * shades they paint pilots in, and whether the sounds that matter are also
 * shown on screen (SOUND CUES, see SoundCues). */
struct EffectPreferences {
  FlashPreference flashes{FlashPreference::Full};
  PilotShades shades{STANDARD_PILOT_SHADES};
  bool soundCues{false};
};

}  // namespace rtype::client
