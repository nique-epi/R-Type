#pragma once

#include <string_view>

namespace rtype::client {

/** @brief A mark of a text, such as {BEST} or {COUNT}, and what replaces it. */
struct TextMark {
  std::string_view mark;
  std::string_view value;
};

}  // namespace rtype::client
