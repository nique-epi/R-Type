#pragma once

#include <string>
#include "HelpWordLook.hpp"

namespace rtype::client {

/** @brief A word of the bubble, its key already named: its text, how it is
 * written, and the punctuation written plain right after it. */
struct HelpWord {
  std::string text;
  HelpWordLook look{};
  std::string suffix;
};

}  // namespace rtype::client
