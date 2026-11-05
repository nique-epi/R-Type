#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::engine {

/** @brief How many words follow `plugin` and `plugin-folder`. */
constexpr std::size_t PLUGIN_ARGUMENT_COUNT = 1;

/** @brief The characters, besides letters and digits, a plugin name may hold.
 */
constexpr std::string_view PLUGIN_NAME_PUNCTUATION = "-_";

/** @brief What a malformed `plugin` line is told to look like. */
constexpr std::string_view PLUGIN_LINE_SHAPE = "plugin <name>";

/** @brief What a malformed `plugin-folder` line is told to look like. */
constexpr std::string_view PLUGIN_FOLDER_LINE_SHAPE = "plugin-folder <folder>";

/** @brief Why a plugin line is refused once the modules are listed. */
constexpr std::string_view PLUGIN_AFTER_INSTALL_REASON =
    "plugins load before the modules install";

/** @brief Why a plugin-folder line is refused after a plugin line. */
constexpr std::string_view FOLDER_AFTER_PLUGIN_REASON =
    "plugin-folder comes before every plugin line";

}  // namespace rtype::engine
