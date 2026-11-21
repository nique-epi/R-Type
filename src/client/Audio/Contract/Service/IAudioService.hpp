#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "Ambience.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the sound module hands to the rest of the client: the player of
 * the short sounds every screen uses, the theme the intro follows, and the
 * choice between the music outside a game and the music of a game.
 *
 * It exists in two kinds: one that plays, and one that plays nothing, so a
 * client launched without sound runs the same code and no screen asks whether
 * the service is there.
 */
class IAudioService {
 public:
  virtual ~IAudioService() = default;

  /** @returns The player of the short sounds of the client. */
  [[nodiscard]] virtual ISoundPlayer& sounds() = 0;

  /** @returns The theme: the music the intro follows and starts. */
  [[nodiscard]] virtual IMusicPlayer& theme() = 0;

  /** @brief Stops the music of a game and plays the theme, unless it plays
   * already. */
  virtual void playTheme() = 0;

  /** @brief Stops the theme and starts the music of a game a silence later,
   * unless it plays already or is about to. When @p levelMusic names the
   * asset of a music, the level plays it in place of the music of a game,
   * bosses included, until playTheme(); without it, the music the level
   * named last in the game plays, or the music of a game. */
  virtual void playGameMusic(const std::optional<std::string>& levelMusic) = 0;

  /** @brief Stops the music of a game at once, without fade, and cancels the
   * silence before it: playGameMusic() starts it again GAME_MUSIC_SILENCE
   * later. The theme is left as it is.
   * @returns False, stopping nothing, while the level plays a music of its
   * own, which a boss leaves playing. */
  [[nodiscard]] virtual bool silenceMusic() = 0;

  /** @brief Makes @p music the music of the boss (BOSS_MUSIC_ID when empty),
   * played in place of the one that plays when it differs, then stops the
   * music of a game and the theme, and starts the music of the boss
   * BOSS_ALARM_DURATION later, unless it plays already or is about to. Over
   * a music a level names, which no alarm silenced, it starts at once.
   * playTheme() and playGameMusic() stop it. */
  virtual void playBossMusic(std::string_view music) = 0;

  /** @brief Plays @p ambience under the theme, and stops the one that
   * played; Ambience::None leaves only the music. Ambience::Shop,
   * Ambience::ShopRadio and Ambience::ShopDance play from the start of
   * their music and pause the theme, which plays on from where it was once
   * another ambience is set. */
  virtual void setAmbience(Ambience ambience) = 0;

  /** @returns The ambience that plays now. */
  [[nodiscard]] virtual Ambience ambience() const = 0;

  /** @brief Reads the sounds and opens the music files, once the assets
   * folder is known; a music without file stays silent. */
  virtual void open() = 0;

  /** @brief Called each frame: the silence before the music of a game moves
   * on by @p elapsed. */
  virtual void update(engine::Duration elapsed) = 0;
};

}  // namespace rtype::client
