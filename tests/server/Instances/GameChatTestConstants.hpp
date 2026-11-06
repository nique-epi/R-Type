#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The line a player sends in the tests of a whole game. */
constexpr std::uint16_t CHAT_LINE_NUMBER = 1;
constexpr std::string_view CHAT_TEXT = "ready?";

}  // namespace rtype::server::testing
