#pragma once

namespace rtype::client {

struct PlayerSettings;

/** @returns The volume the music plays at, from 0 to 100 as SFML takes it:
 * the MUSIC volume of the player, 0 while MUTE ALL is on. */
[[nodiscard]] float musicVolumeOf(const PlayerSettings& settings);

/** @returns The volume a sound plays at, from 0 to 100 as SFML takes it: the
 * EFFECTS volume of the player, 0 while MUTE ALL is on. */
[[nodiscard]] float effectsVolumeOf(const PlayerSettings& settings);

}  // namespace rtype::client
