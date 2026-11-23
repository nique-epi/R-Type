#include "OpeningSky.hpp"
#include <optional>
#include <string>
#include "AnnouncedStart.hpp"
#include "GameStarted.hpp"
#include "IAudioService.hpp"
#include "ISky.hpp"
#include "SkyLevel.hpp"

namespace rtype::client {

namespace {

/** @returns What the sky shows of the level @p started tells: its scroll
 * does not move before the game is in progress. */
SkyLevel skyLevelOf(const network::GameStarted& started) {
  return SkyLevel{.biome = started.biome,
                  .scrollPosition = started.scrollPosition,
                  .scrollSpeed = 0.0F,
                  .biomeSince = started.biomeSince};
}

}  // namespace

bool showLevelWithItsMusic(ISky& sky, IAudioService& audio,
                           const SkyLevel& level) {
  sky.showLevel(level);
  const std::optional<std::string> music = sky.biomeMusic();
  if (music.has_value()) {
    audio.playGameMusic(music);
  }
  return music.has_value();
}

void openTheLevel(const AnnouncedStart& start, ISky& sky,
                  IAudioService& audio) {
  if (!start.started().has_value()) {
    audio.playGameMusic(std::nullopt);
    return;
  }
  if (!showLevelWithItsMusic(sky, audio, skyLevelOf(*start.started()))) {
    audio.playGameMusic(std::nullopt);
  }
}

}  // namespace rtype::client
