#include "LevelMusic.hpp"
#include <string>
#include <utility>
#include "AudioConstants.hpp"
#include "IAudioService.hpp"
#include "ISky.hpp"
#include "LevelStatus.hpp"

namespace rtype::client {

LevelMusic::LevelMusic(const ISky& sky, IAudioService& audio)
    : sky_(&sky), audio_(&audio) {}

void LevelMusic::follow(const network::LevelStatus& status) {
  std::string music = sky_->musicOf(status.biome);
  if (music.empty()) {
    music = GAME_MUSIC_ID;
  }
  if (played_ == music) {
    return;
  }
  audio_->playGameMusic(music);
  played_ = std::move(music);
}

}  // namespace rtype::client
