#pragma once

#include <sysdir.h>
#include <filesystem>
#include <optional>
#include <string_view>

namespace rtype::client {

/**
 * @brief The folder @p child of the folder of the user's domain that sysdir
 * gives for @p directory, its leading ~ replaced by $HOME, or nothing when
 * sysdir gives none or the home is unset or empty.
 */
[[nodiscard]] std::optional<std::filesystem::path> macOsUserFolder(
    sysdir_search_path_directory_t directory, std::string_view child);

}  // namespace rtype::client
