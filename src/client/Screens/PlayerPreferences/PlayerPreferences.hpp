#pragma once

#include "FlashPreference.hpp"
#include "SkyLight.hpp"

namespace rtype::client {

struct PlayerSettings;

/** @returns The version of the effects the settings ask for: the one without
 * flash when the player turned REDUCE FLASHES on. */
[[nodiscard]] FlashPreference flashPreferenceOf(const PlayerSettings& settings);

/** @returns How the sky shines for the settings: steady when the player
 * turned REDUCE FLASHES on, twinkling otherwise. */
[[nodiscard]] SkyLight skyLightOf(const PlayerSettings& settings);

}  // namespace rtype::client
