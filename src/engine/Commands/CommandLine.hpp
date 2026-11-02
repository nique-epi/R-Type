#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace rtype::engine {

/**
 * @brief Cuts a line of commands into its words.
 *
 * Spaces separate the words; a pair of quotes keeps the spaces between them in
 * one word; a comment mark outside quotes ends the line. A quote left open
 * ends with the line.
 *
 * @returns The words of the line; none for a blank line or a comment.
 */
[[nodiscard]] std::vector<std::string> splitCommandLine(std::string_view line);

}  // namespace rtype::engine
