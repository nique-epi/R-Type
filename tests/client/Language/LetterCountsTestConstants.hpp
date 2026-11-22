#pragma once

#include <cstddef>
#include <string_view>
#include "Translation.hpp"

/** @brief A word of four letters whose third takes two bytes of UTF-8, and a
 * translation whose French is longer than its English. */
constexpr std::string_view FOUR_LETTERS_ONE_ACCENT = "PRÊT";
constexpr std::size_t FOUR_LETTERS = 4;
constexpr rtype::client::Translation LONGER_IN_FRENCH{.english = "READY",
                                                      .french = "PRÉPARÉ"};
constexpr std::size_t LETTERS_OF_PREPARE = 7;
