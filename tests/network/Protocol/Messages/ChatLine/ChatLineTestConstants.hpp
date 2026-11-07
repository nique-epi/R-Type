#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief A line number and a text of two bytes, "gg" (0x67 0x67), as the
 * protocol page writes a line. */
constexpr std::uint16_t SOME_LINE_NUMBER = 7;
constexpr std::string_view SOME_TEXT = "gg";

/** @brief The byte a long text is made of, and a control character no text may
 * hold. */
constexpr char TEXT_FILLER = 'a';
constexpr std::string_view TEXT_WITH_A_NEWLINE = "g\ng";

}  // namespace rtype::network::testing
