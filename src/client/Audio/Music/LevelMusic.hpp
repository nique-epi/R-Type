#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include "IMusicPlayer.hpp"
#include "MusicPlayer.hpp"

namespace rtype::client {

class AssetLibrary;
class SettingsService;

/**
 * @brief The musics the backgrounds of the levels name, each opened the
 * first time a level asks for it and kept for the rest of the run, so the
 * player a Soundtrack holds never dies under it. Each one plays its whole
 * file in a loop, at the MUSIC volume of the player.
 */
class LevelMusic {
 public:
  /** @param settings Holds the volumes; it must outlive the musics. */
  explicit LevelMusic(SettingsService& settings);

  LevelMusic(const LevelMusic&) = delete;
  LevelMusic& operator=(const LevelMusic&) = delete;
  LevelMusic(LevelMusic&&) = delete;
  LevelMusic& operator=(LevelMusic&&) = delete;

  /** @brief Stops the settings from telling the musics. */
  ~LevelMusic();

  /** @returns The player of the music @p assetId, opened from @p library the
   * first time it is asked for; silent when the file is missing. */
  [[nodiscard]] IMusicPlayer& playerOf(const std::string& assetId,
                                       AssetLibrary& library);

 private:
  SettingsService* settings_;
  std::map<std::string, std::unique_ptr<MusicPlayer>, std::less<>> players_;
};

}  // namespace rtype::client
