#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/** @brief The sample game of --end-screen-demo, the one of the design system:
 * three pilots, the first of them this client. */
constexpr std::array<std::string_view, 3> DEMO_NICKNAMES{
    "ACE", "CAPTAIN_STARDUST", "NOVA"};
constexpr std::array<std::uint32_t, 3> DEMO_SCORES{12450, 31800, 8200};
constexpr std::uint8_t DEMO_LOCAL_PLAYER = 1;

}  // namespace rtype::client
