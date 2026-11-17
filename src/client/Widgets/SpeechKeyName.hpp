#pragma once

#include <string>
#include <string_view>

namespace rtype::client {

/** @brief A word a sentence may write between braces, and the name of the key
 * its keycap shows in its place: "{SHOOT}" shows the key bound to shooting. */
struct SpeechKeyName {
  std::string_view token;
  std::string name;
};

}  // namespace rtype::client
