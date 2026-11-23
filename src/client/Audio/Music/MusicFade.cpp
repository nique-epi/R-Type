#include "MusicFade.hpp"
#include "AudioConstants.hpp"
#include "IMusicPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void MusicFade::begin(IMusicPlayer& music, engine::Duration length) {
  cancel();
  music_ = &music;
  length_ = length;
  elapsed_ = engine::Duration::zero();
  music.setFade(MUSIC_FADE_SILENT);
  music.start();
}

void MusicFade::advance(engine::Duration elapsed) {
  if (music_ == nullptr) {
    return;
  }
  elapsed_ += elapsed;
  if (elapsed_ >= length_) {
    cancel();
    return;
  }
  music_->setFade(static_cast<float>(elapsed_.count()) /
                  static_cast<float>(length_.count()));
}

void MusicFade::cancel() {
  if (music_ == nullptr) {
    return;
  }
  music_->setFade(MUSIC_FADE_FULL);
  music_ = nullptr;
}

}  // namespace rtype::client
