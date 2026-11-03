#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace rtype::engine {

/**
 * @brief Cuts a line of commands into its words.
 *
 * Spaces separate the words; a pair of quotes keeps the spaces between them in
 * one word; a comment mark outside quotes ends the line. Inside quotes, a
 * backslash before a quote or a backslash makes that character part of the
 * word; before any other character it is a plain backslash, as it is outside
 * quotes. A quote left open ends with the line.
 *
 * @returns The words of the line; none for a blank line or a comment.
 */
[[nodiscard]] std::vector<std::string> splitCommandLine(std::string_view line);

}  // namespace rtype::engine
