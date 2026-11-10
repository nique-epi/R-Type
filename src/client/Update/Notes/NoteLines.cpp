#include "NoteLines.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "UpdateConstants.hpp"

namespace rtype::client {

namespace {

/** @returns @p line without its surrounding blanks and its list mark. */
std::string_view textOf(std::string_view line) {
  const std::size_t first = line.find_first_not_of(NOTE_BLANKS);
  if (first == std::string_view::npos) {
    return {};
  }
  line.remove_prefix(first);
  if (std::ranges::find(LIST_ITEM_MARKS, line.front()) !=
      LIST_ITEM_MARKS.end()) {
    line.remove_prefix(1);
  }
  const std::size_t start = line.find_first_not_of(NOTE_BLANKS);
  if (start == std::string_view::npos) {
    return {};
  }
  line.remove_prefix(start);
  return line.substr(0, line.find_last_not_of(NOTE_BLANKS) + 1);
}

/** @returns The characters of @p text the font draws, at most
 * MAXIMUM_NOTE_SIZE of them. */
std::string printableOf(std::string_view text) {
  std::string printable;
  for (const char character : text) {
    if (printable.size() == MAXIMUM_NOTE_SIZE) {
      break;
    }
    if (character >= FIRST_PRINTABLE_CHARACTER &&
        character <= LAST_PRINTABLE_CHARACTER) {
      printable.push_back(character);
    }
  }
  return printable;
}

}  // namespace

std::vector<std::string> noteLinesOf(std::string_view markdown) {
  std::vector<std::string> lines;
  while (!markdown.empty() && lines.size() < MAXIMUM_NOTE_LINES) {
    const std::size_t end = markdown.find(NOTE_LINE_BREAK);
    const std::string line = printableOf(textOf(markdown.substr(0, end)));
    if (!line.empty()) {
      lines.push_back(line);
    }
    markdown.remove_prefix(end == std::string_view::npos ? markdown.size()
                                                         : end + 1);
  }
  return lines;
}

}  // namespace rtype::client
