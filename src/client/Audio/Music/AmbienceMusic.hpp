#pragma once

#include <array>
#include <vector>
#include "Ambience.hpp"
#include "AmbienceMusicConstants.hpp"
#include "MusicPlayer.hpp"

namespace rtype::client {

class AssetLibrary;
class SettingsService;

/** @returns Whether @p ambience plays instead of the theme, which is paused
 * for as long as it plays: the musics of the shop and of the casino. */
[[nodiscard]] bool replacesTheTheme(Ambience ambience);

/**
 * @brief The musics and the ambiences that color a screen, one Ambience at a
 * time: each a MusicPlayer that follows the volumes of the player, started
 * when its ambience is chosen and stopped when another one is. The hangar and
 * the hall of the casino play two at once, a crowd under their sound.
 */
class AmbienceMusic {
 public:
  /** @param settings The settings whose volumes the players follow; they
   * must outlive the music. */
  explicit AmbienceMusic(SettingsService& settings);

  AmbienceMusic(const AmbienceMusic&) = delete;
  AmbienceMusic& operator=(const AmbienceMusic&) = delete;
  AmbienceMusic(AmbienceMusic&&) = delete;
  AmbienceMusic& operator=(AmbienceMusic&&) = delete;
  ~AmbienceMusic();

  /** @brief Finds the file of every music in @p library. */
  void open(AssetLibrary& library);
  /** @brief Stops what plays, then starts what @p ambience plays. */
  void play(Ambience ambience);

 private:
  /** @returns Every player of the ambiences, in an array, so the destructor
   * that walks them allocates nothing. */
  [[nodiscard]] std::array<MusicPlayer*, AMBIENCE_PLAYER_COUNT> allPlayers();
  /** @returns The players @p ambience starts; none for Ambience::None. */
  [[nodiscard]] std::vector<MusicPlayer*> playersOf(Ambience ambience);

  SettingsService* settings_;
  MusicPlayer space_;
  MusicPlayer hangar_;
  MusicPlayer hangarCrowd_;
  MusicPlayer shop_;
  MusicPlayer shopRadio_;
  MusicPlayer shopDance_;
  MusicPlayer casinoFront_;
  MusicPlayer casinoHall_;
  MusicPlayer casinoCrowd_;
  MusicPlayer casinoTables_;
};

}  // namespace rtype::client
