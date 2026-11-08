#pragma once

#include <array>
#include <string_view>

/** @brief The tables of the waves file of the example resource, in
 * alphabetical order. */
constexpr std::array<std::string_view, 3> EXAMPLE_RESOURCE_DIFFICULTIES{
    "easy", "hard", "normal"};
