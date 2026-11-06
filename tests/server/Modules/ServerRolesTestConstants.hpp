#pragma once

#include <array>
#include <string_view>

/** @brief A `set` line on a name no module declares: its refusal lists the
 * settings the program declared. */
constexpr std::string_view UNDECLARED_SETTING_LINE =
    "set no-module-declares-this-setting 0";

/** @brief What comes before the list of the declared settings in the refusal
 * of an unknown one, and what separates two names of that list. */
constexpr std::string_view KNOWN_SETTINGS_MARKER = "known settings: ";
constexpr std::string_view KNOWN_SETTINGS_SEPARATOR = ", ";

/** @brief What a line of server.cfg.example may start with before its
 * command: a commented example reads `# set <name> <value>`. */
constexpr std::string_view COMMENT_PREFIX_CHARACTERS = "# ";

/** @brief The settings the supervisor gives each game on its command line,
 * and not through the lines of server.cfg: its own port and its own
 * database. */
constexpr std::array<std::string_view, 2> SETTINGS_A_GAME_IS_LAUNCHED_WITH{
    "database", "port"};

/** @brief A setting server.cfg.example only shows commented out, since it is
 * refused unless the game-statistics plugin is loaded. */
constexpr std::string_view COMMENTED_EXAMPLE_SETTING = "game-statistics-file";

/** @brief The ports of the game the tests list the modules of, and of the
 * supervisor it reports to; nothing is bound on either. */
constexpr std::string_view ROLE_GAME_PORT = "4243";
constexpr std::string_view ROLE_SUPERVISOR_PORT = "4242";
