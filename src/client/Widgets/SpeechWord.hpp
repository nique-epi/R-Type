#pragma once

#include <string>
#include "SpeechWordLook.hpp"

namespace rtype::client {

/** @brief A word of a speech bubble, its key already named: its text, how it is
 * written, and the punctuation written plain right after it. */
struct SpeechWord {
  std::string text;
  SpeechWordLook look{};
  std::string suffix;
};

}  // namespace rtype::client
