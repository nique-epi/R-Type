#pragma once

/** @brief The names of the test plugins the lines of the tests load. */
constexpr const char* PROBE_PLUGIN_NAME = "probe";
constexpr const char* ENTRYLESS_PLUGIN_NAME = "entryless";
constexpr const char* FUTURE_PLUGIN_NAME = "future";

/** @brief A plugin no file is called after. */
constexpr const char* GHOST_PLUGIN_NAME = "ghost";

/** @brief A name that tries to leave the plugin folder. */
constexpr const char* PLUGIN_NAME_WITH_A_SLASH = "../probe";

/** @brief The lines of the tests, which a test completes with the folder. */
constexpr const char* PLUGIN_FOLDER_LINE_START = "plugin-folder ";
constexpr const char* PLUGIN_LINE_START = "plugin ";

/** @brief A folder no test plugin is in. */
constexpr const char* OTHER_PLUGIN_FOLDER = "/an-other-plugin-folder";

/** @brief How many events the tests publish for a plugin to count. */
constexpr int ONE_EVENT = 1;

/** @brief How many events a plugin counts when it is turned off or not loaded.
 */
constexpr int NO_EVENT = 0;

/** @brief What the refusal of a plugin-folder line after a plugin line says. */
constexpr const char* FOLDER_AFTER_PLUGIN_REFUSAL =
    "plugin-folder comes before every plugin line";
