#pragma once

#include <string_view>

namespace rtype::server::testing {

/** @brief A comment line a test adds at the end of a shipped sheet. */
constexpr std::string_view A_COMMENT_LINE_OF_A_SHEET = "# one more line\n";

}  // namespace rtype::server::testing
