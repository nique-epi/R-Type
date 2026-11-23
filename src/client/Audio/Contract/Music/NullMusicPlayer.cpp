#include "NullMusicPlayer.hpp"
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

void NullMusicPlayer::start() {}

void NullMusicPlayer::stop() {}

void NullMusicPlayer::pause() {}

void NullMusicPlayer::setFade([[maybe_unused]] float level) {}

std::optional<engine::Duration> NullMusicPlayer::playingOffset() const {
  return std::nullopt;
}

}  // namespace rtype::client
