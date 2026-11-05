#pragma once

#include <array>
#include <string_view>

/** @brief The module the parts are provided by, named in the errors only. */
constexpr std::string_view PROVIDING_MODULE_NAME = "connection";

/** @brief The stages of the loop: the connection adds no work to them when it
 * provides its parts. */
constexpr std::array<std::string_view, 1> SOME_LOOP_STAGES{"update"};
