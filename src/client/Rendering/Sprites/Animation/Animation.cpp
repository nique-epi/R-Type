#include "Animation.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

std::optional<game::SpriteFrame> Animation::frameAt(
    engine::Duration elapsed) const {
  if (frames.empty() || frameDuration <= engine::Duration::zero()) {
    return std::nullopt;
  }
  const auto frameCount = static_cast<engine::Duration::rep>(frames.size());
  const engine::Duration::rep shown = elapsed / frameDuration;
  if (playback == Playback::Loop) {
    return frames[static_cast<std::size_t>(((shown % frameCount) + frameCount) %
                                           frameCount)];
  }
  return frames[static_cast<std::size_t>(
      std::clamp<engine::Duration::rep>(shown, 0, frameCount - 1))];
}

bool Animation::hasEnded(engine::Duration elapsed) const {
  return playback == Playback::Once && elapsed >= cycle();
}

engine::Duration Animation::cycle() const {
  return frameDuration * static_cast<engine::Duration::rep>(frames.size());
}

}  // namespace rtype::client
