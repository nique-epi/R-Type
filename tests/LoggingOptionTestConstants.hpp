#pragma once

#include <string_view>

/**
 * @brief A level given after `--log-level`, as a player types it.
 */
constexpr std::string_view DEBUG_LEVEL_ARGUMENT = "debug";

/**
 * @brief `--log-level` misspelled: a logging option that does not exist.
 */
constexpr std::string_view MISSPELLED_LEVEL_OPTION = "--log-levle";
