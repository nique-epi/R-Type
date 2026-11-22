#pragma once

#include <string>
#include "TextMark.hpp"

namespace rtype::client {

/** @returns @p text, its first @p mark replaced by its value; unchanged when
 * the mark is not in it. French and English may place a mark at different
 * places of a sentence, so a sentence with a number is one translation with a
 * mark, never two pieces glued around the number. */
[[nodiscard]] std::string withMark(std::string text, TextMark mark);

}  // namespace rtype::client
