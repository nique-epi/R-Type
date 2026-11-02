#include "SettingLine.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include "CommandsConstants.hpp"
#include "EngineException.hpp"

namespace rtype::engine {

namespace {

constexpr char ASSIGNMENT_MARK = '=';

bool needsQuotes(std::string_view value) {
  return value.empty() || std::ranges::any_of(value, [](char character) {
           return std::isspace(static_cast<unsigned char>(character)) != 0 ||
                  character == COMMENT_START;
         });
}

}  // namespace

std::string settingLine(std::string_view name, std::string_view value) {
  if (value.find(QUOTE) != std::string_view::npos) {
    throw CommandRefusedException("a value cannot hold a quote");
  }
  std::string line =
      std::string(SET_COMMAND_NAME) + " " + std::string(name) + " ";
  if (needsQuotes(value)) {
    line += QUOTE;
    line += value;
    line += QUOTE;
  } else {
    line += value;
  }
  return line;
}

std::string settingLineFromAssignment(std::string_view assignment) {
  const std::size_t mark = assignment.find(ASSIGNMENT_MARK);
  if (mark == std::string_view::npos || mark == 0) {
    throw CommandRefusedException("expected: <setting>=<value>");
  }
  return settingLine(assignment.substr(0, mark), assignment.substr(mark + 1));
}

}  // namespace rtype::engine
