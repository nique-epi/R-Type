#pragma once

#include <cstddef>
#include <span>
#include "VoiceState.hpp"

namespace rtype::client {

/**
 * @returns The voice the next sound plays on: a new one, numbered
 * voices.size(), while fewer than @p voiceLimit exist; else the first voice
 * that stopped playing; else the voice whose sound started first, which it
 * cuts. With a positive limit and at most @p voiceLimit voices, it never
 * returns @p voiceLimit or more.
 */
[[nodiscard]] std::size_t voiceForNextSound(std::span<const VoiceState> voices,
                                            std::size_t voiceLimit);

}  // namespace rtype::client
