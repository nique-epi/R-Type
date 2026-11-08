#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::engine {

/** @brief The command of a file of commands that starts a resource:
 * `ensure <name>`. */
constexpr std::string_view ENSURE_COMMAND_NAME = "ensure";

/** @brief The words of an `ensure` line: the command and the name. */
constexpr std::size_t ENSURE_LINE_WORDS = 2;

/** @brief The folder of the resources, beside the assets folder. */
constexpr std::string_view RESOURCE_FOLDER_NAME = "resources";

/** @brief The file each resource folder describes itself in. */
constexpr std::string_view RESOURCE_MANIFEST_FILE_NAME = "resource.toml";

/** @brief The longest name of a resource, the name of its folder. */
constexpr std::size_t MAXIMUM_RESOURCE_NAME_LENGTH = 32;

/** @brief The characters a resource name may hold beside lowercase letters and
 * digits. */
constexpr std::array RESOURCE_NAME_PUNCTUATION{'-', '_'};

/** @brief What a resource says it adds, one line for the journal. */
constexpr std::string_view RESOURCE_DESCRIPTION_KEY = "description";

/** @brief The resources a resource needs, started before it. */
constexpr std::string_view RESOURCE_DEPENDENCIES_KEY = "depends";

/** @brief Every key a manifest may hold. */
constexpr std::array<std::string_view, 2> RESOURCE_MANIFEST_KEYS{
    RESOURCE_DESCRIPTION_KEY, RESOURCE_DEPENDENCIES_KEY};

}  // namespace rtype::engine
