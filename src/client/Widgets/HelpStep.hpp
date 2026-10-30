#pragma once

#include <string_view>
#include "HelpPart.hpp"
#include "HelpScene.hpp"

namespace rtype::client {

/**
 * @brief One step of the help: what it shows, the part it lights, and the
 * sentence of the professor.
 *
 * In the sentence, a word between braces is a keycap: one of HELP_KEY_TOKENS
 * names the key the player bound to that action, any other word is the name
 * of a fixed key, such as {ESC}. A word between stars is a word of the
 * interface, written in cyan. Punctuation right after either is written plain.
 */
struct HelpStep {
  HelpScene scene{};
  HelpPart part{};
  std::string_view text;
};

}  // namespace rtype::client
