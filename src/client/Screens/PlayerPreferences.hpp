#pragma once

#include "FlashPreference.hpp"
#include "PilotShades.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

struct PlayerSettings;

/** @returns The version of the effects the settings ask for: the one without
 * flash when the player turned REDUCE FLASHES on. */
[[nodiscard]] FlashPreference flashPreferenceOf(const PlayerSettings& settings);

/** @returns The shades the color mode of the settings paints pilots in, and
 * the debris ramps made of them; constants, which outlive every caller. */
[[nodiscard]] PilotShades pilotShadesOf(const PlayerSettings& settings);

/** @returns The palette the color mode of the settings paints pilots in; a
 * constant, which outlives every caller. */
[[nodiscard]] const PlayerPalette& playerPaletteOf(
    const PlayerSettings& settings);

}  // namespace rtype::client
