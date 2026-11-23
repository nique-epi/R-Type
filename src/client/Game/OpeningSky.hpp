#pragma once

#include "AnnouncedStart.hpp"
#include "IAudioService.hpp"
#include "ISky.hpp"
#include "SkyLevel.hpp"

namespace rtype::client {

/**
 * @brief Shows @p level in @p sky, then plays the music its background names
 * in place of the music of a game, when it names one; the music playing is
 * left as it is otherwise. Every level the sky of a game shows goes through
 * it: the one a GameStarted tells and each LevelStatus.
 * @returns Whether the background names a music.
 */
bool showLevelWithItsMusic(ISky& sky, IAudioService& audio,
                           const SkyLevel& level);

/**
 * @brief Starts the game a screen is about to load: shows in @p sky the
 * level the GameStarted of @p start tells, its scroll still, and plays the
 * music of its background, the music of a game when that background names
 * none. Without a GameStarted, its copies lost and the game started by a
 * WorldState, the sky is left as it is and the music of a game plays: the
 * first LevelStatus brings the level.
 *
 * The game screen and the loading screen before it both call it with the
 * same start, so the second call changes nothing.
 */
void openTheLevel(const AnnouncedStart& start, ISky& sky, IAudioService& audio);

}  // namespace rtype::client
