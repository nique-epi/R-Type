#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include "MacOsUserDirectory.hpp"

namespace rtype::client {

/**
 * @brief The folder @p child of the folder of the user's domain that sysdir
 * gives for @p directory, its leading ~ replaced by $HOME, or nothing when
 * sysdir gives none or the home is unset or empty. Defined on macOS only.
 */
[[nodiscard]] std::optional<std::filesystem::path> macOsUserFolder(
    MacOsUserDirectory directory, std::string_view child);

}  // namespace rtype::client
