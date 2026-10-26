#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the choice of a voice reads of a voice: whether it still
 * plays, and the rank of its sound among the sounds started, 0 for the
 * first. */
struct VoiceState {
  bool playing{};
  std::uint64_t startRank{};
};

}  // namespace rtype::client
