#include "NullMusicPlayer.hpp"
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

void NullMusicPlayer::start() {}

void NullMusicPlayer::stop() {}

std::optional<engine::Duration> NullMusicPlayer::playingOffset() const {
  return std::nullopt;
}

}  // namespace rtype::client
