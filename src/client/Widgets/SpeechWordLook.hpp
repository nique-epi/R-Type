#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How a word of a speech bubble is written: plain, in the cyan of the
 * words of the interface, or as a keycap. */
enum class SpeechWordLook : std::uint8_t {
  Plain,
  Highlighted,
  KeyCap,
};

}  // namespace rtype::client
