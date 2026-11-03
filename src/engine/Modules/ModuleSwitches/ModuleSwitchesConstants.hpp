#pragma once

#include <string_view>

namespace rtype::engine {

/**
 * @brief The command that turns a module of the program on or off.
 */
constexpr std::string_view MODULE_COMMAND_NAME = "module";

/**
 * @brief The word that turns a module on.
 */
constexpr std::string_view SWITCH_ON_WORD = "on";

/**
 * @brief The word that turns a module off.
 */
constexpr std::string_view SWITCH_OFF_WORD = "off";

}  // namespace rtype::engine
