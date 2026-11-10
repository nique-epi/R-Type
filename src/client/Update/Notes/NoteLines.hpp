#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace rtype::client {

/** @returns The lines WHAT'S NEW shows of the notes @p markdown: each line
 * that holds text, its list mark and surrounding blanks dropped, keeping only
 * the characters from FIRST_PRINTABLE_CHARACTER to LAST_PRINTABLE_CHARACTER,
 * cut to MAXIMUM_NOTE_SIZE; the first MAXIMUM_NOTE_LINES of them. */
[[nodiscard]] std::vector<std::string> noteLinesOf(std::string_view markdown);

}  // namespace rtype::client
