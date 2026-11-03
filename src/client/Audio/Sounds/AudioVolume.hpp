#pragma once

#include "SettingId.hpp"

namespace rtype::client {

struct PlayerSettings;

/** @returns The volume the music plays at, from 0 to 100 as SFML takes it:
 * the MUSIC volume of the player, 0 while MUTE ALL is on. */
[[nodiscard]] float musicVolumeOf(const PlayerSettings& settings);

/** @returns The volume a sound plays at, from 0 to 100 as SFML takes it: the
 * EFFECTS volume of the player, 0 while MUTE ALL is on. */
[[nodiscard]] float effectsVolumeOf(const PlayerSettings& settings);

/** @returns Whether a change of @p changed moves the volume of the music: the
 * MUSIC volume, or MUTE ALL. */
[[nodiscard]] bool changesMusicVolume(SettingId changed);

/** @returns Whether a change of @p changed moves the volume of the sounds: the
 * EFFECTS volume, or MUTE ALL. */
[[nodiscard]] bool changesEffectsVolume(SettingId changed);

}  // namespace rtype::client
