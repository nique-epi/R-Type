#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief The continues a server tells are left, one status after the
 * other. */
constexpr std::uint8_t THREE_CONTINUES_TOLD = 3;
constexpr std::uint8_t TWO_CONTINUES_TOLD = 2;
constexpr std::uint8_t ONE_CONTINUE_TOLD = 1;
constexpr std::uint8_t NO_CONTINUE_TOLD = 0;

/** @brief The banner as the picture the team chose sets it: CONTINUE over
 * the continues left, for two seconds, the line of a team without any in
 * the orange of the warnings (WARNING of StatusBandTestConstants.hpp). */
constexpr std::string_view CONTINUE_TITLE_ON_SCREEN = "CONTINUE";
constexpr std::string_view TWO_LEFT_ON_SCREEN = "2 LEFT";
constexpr std::string_view NONE_LEFT_ON_SCREEN = "NO CONTINUE LEFT";
constexpr std::chrono::nanoseconds TIME_OF_THE_CONTINUE_BANNER =
    std::chrono::seconds(2);
constexpr std::chrono::nanoseconds MOST_OF_THE_CONTINUE_BANNER =
    std::chrono::milliseconds(1500);

/** @brief The rules of the banner: one above its lines, one under. */
constexpr std::size_t RULES_OF_THE_CONTINUE_BANNER = 2;
