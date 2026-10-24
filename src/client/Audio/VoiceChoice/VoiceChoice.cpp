#include "VoiceChoice.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include "VoiceState.hpp"

namespace rtype::client {

std::size_t voiceForNextSound(std::span<const VoiceState> voices,
                              std::size_t voiceLimit) {
  if (voices.size() < voiceLimit) {
    return voices.size();
  }
  const auto stopped = std::ranges::find(voices, false, &VoiceState::playing);
  if (stopped != voices.end()) {
    return static_cast<std::size_t>(stopped - voices.begin());
  }
  const auto oldest =
      std::ranges::min_element(voices, {}, &VoiceState::startRank);
  return static_cast<std::size_t>(oldest - voices.begin());
}

}  // namespace rtype::client
