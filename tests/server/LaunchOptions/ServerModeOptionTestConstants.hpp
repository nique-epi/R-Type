#pragma once

#include <string_view>

/** @brief The option that chooses the program, and the names it takes. */
constexpr std::string_view MODE = "--mode";
constexpr std::string_view GAME_MODE = "game";
constexpr std::string_view MASTER_MODE = "master";
constexpr std::string_view SUPERVISOR_MODE = "supervisor";

/** @brief A name that is no mode of the binary. */
constexpr std::string_view UNKNOWN_MODE = "orchestra";

/** @brief The name of a server that hosts several games in one process, which
 * the binary does not run. */
constexpr std::string_view SERVER_MODE = "server";

/** @brief An option of either program, and its value. */
constexpr std::string_view PORT = "-p";
constexpr std::string_view SOME_PORT = "4243";
