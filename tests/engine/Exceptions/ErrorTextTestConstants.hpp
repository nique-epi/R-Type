#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "WholeNumberRange.hpp"

/** @brief A place in a data file, and how the errors write it. */
constexpr std::string_view SHEET_FILE = "sheet.toml";
constexpr std::size_t SIDE_LINE = 2;
constexpr std::size_t SIDE_COLUMN = 8;
constexpr std::string_view SIDE_KEY_NAME = "gameplay.side";
constexpr std::string_view SIDE_PLACE_TEXT = "sheet.toml:2:8";

/** @brief Two names, and how a list writes them. */
constexpr std::string_view PLAYER = "player";
constexpr std::string_view ENEMY = "enemy";
constexpr std::string_view TWO_NAMES_TEXT = "player, enemy";

/** @brief Three choices a key accepts, as a constants header lists them, and
 * how a list writes them. */
constexpr std::array<std::string_view, 3> THREE_CHOICES{"ship", "steady",
                                                        "still"};
constexpr std::string_view THREE_CHOICES_TEXT = "ship, steady, still";

/** @brief What a list of no name says. */
constexpr std::string_view NO_NAME_TEXT = "none";

/** @brief A range of whole numbers, and how the errors write it. */
constexpr rtype::engine::WholeNumberRange ONE_TO_A_HUNDRED{.lowest = 1,
                                                           .highest = 100};
constexpr std::string_view ONE_TO_A_HUNDRED_TEXT = "1 to 100";
