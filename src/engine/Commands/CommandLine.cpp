#include "CommandLine.hpp"
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "CommandLineConstants.hpp"

namespace rtype::engine {

namespace {

bool isSpace(char character) {
  return std::isspace(static_cast<unsigned char>(character)) != 0;
}

bool isEscapable(char character) {
  return character == QUOTE || character == ESCAPE;
}

}  // namespace

std::vector<std::string> splitCommandLine(std::string_view line) {
  std::vector<std::string> words;
  std::string word;
  bool inQuotes = false;
  bool inWord = false;
  for (std::size_t position = 0; position < line.size(); ++position) {
    const char character = line[position];
    if (!inQuotes && character == COMMENT_START) {
      break;
    }
    if (inQuotes && character == ESCAPE && position + 1 < line.size() &&
        isEscapable(line[position + 1])) {
      ++position;
      word.push_back(line[position]);
    } else if (character == QUOTE) {
      inQuotes = !inQuotes;
      inWord = true;
    } else if (!inQuotes && isSpace(character)) {
      if (inWord) {
        words.push_back(std::exchange(word, std::string{}));
        inWord = false;
      }
    } else {
      word.push_back(character);
      inWord = true;
    }
  }
  if (inWord) {
    words.push_back(std::move(word));
  }
  return words;
}

}  // namespace rtype::engine
