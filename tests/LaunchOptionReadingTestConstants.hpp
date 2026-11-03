#pragma once

#include <cstddef>

/**
 * @brief How many arguments the reader of an option takes, as
 * engine::LaunchOptionReader requires: an option and its value, an option
 * alone, and nothing for an argument that is none of its options.
 */
constexpr std::size_t AN_OPTION_AND_ITS_VALUE = 2;
constexpr std::size_t AN_OPTION_ALONE = 1;
constexpr std::size_t NO_ARGUMENT_TAKEN = 0;
