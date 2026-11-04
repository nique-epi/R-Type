#pragma once

#include <string_view>

/** @brief The option that chooses the program, and the names it takes. */
constexpr std::string_view MODE = "--mode";
constexpr std::string_view SERVER_MODE = "server";
constexpr std::string_view MASTER_MODE = "master";

/** @brief A name that is no mode of the binary. */
constexpr std::string_view UNKNOWN_MODE = "orchestra";

/** @brief An option of either program, and its value. */
constexpr std::string_view PORT = "-p";
constexpr std::string_view SOME_PORT = "4243";
