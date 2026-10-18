#pragma once

#include <filesystem>
#include <optional>

namespace rtype::client {

/**
 * @brief Folder where the client keeps the player's files, or nothing when
 * the system does not tell; it may not exist yet.
 *
 * On Linux, the r-type folder of $XDG_CONFIG_HOME, or of $HOME/.config when
 * that variable is unset, empty or not absolute; on macOS, the R-Type folder
 * of the user's Application Support; on Windows, the R-Type folder of the
 * user's local application data.
 */
[[nodiscard]] std::optional<std::filesystem::path> settingsFolder();

}  // namespace rtype::client
