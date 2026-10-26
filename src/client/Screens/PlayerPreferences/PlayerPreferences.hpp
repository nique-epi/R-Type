#pragma once

#include "FlashPreference.hpp"

namespace rtype::client {

struct PlayerSettings;

/** @returns The version of the effects the settings ask for: the one without
 * flash when the player turned REDUCE FLASHES on. */
[[nodiscard]] FlashPreference flashPreferenceOf(const PlayerSettings& settings);

}  // namespace rtype::client
