#pragma once

#include <string_view>
#include "InterfaceConstants.hpp"

namespace rtype::client {

/** @brief Marks of a sentence said in a bubble: a keycap between braces, a
 * word of the interface between stars, the punctuation written plain after
 * either, and the separator of the words. */
constexpr char KEY_CAP_OPENING = '{';
constexpr char KEY_CAP_CLOSING = '}';
constexpr char INTERFACE_WORD_MARK = '*';
constexpr std::string_view SPEECH_PUNCTUATION = ".,:!?";
constexpr char WORD_SEPARATOR = ' ';

/** @brief A line of a bubble every SPEECH_LINE_HEIGHT pixels so a keycap
 * fits, SPEECH_WORD_GAP between two words. */
constexpr int SPEECH_LINE_HEIGHT = 13;
constexpr int SPEECH_WORD_GAP = REGULAR_FONT_ADVANCE;

}  // namespace rtype::client
