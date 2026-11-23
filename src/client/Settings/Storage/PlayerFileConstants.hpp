#pragma once

#include <filesystem>
#include <string_view>

namespace rtype::client {

/** @brief The suffix of the file written next to a file of the player, then
 * renamed over it, so a write cut short never leaves half a file. */
constexpr std::string_view TEMPORARY_PLAYER_FILE_SUFFIX = ".tmp";

/** @brief Who may open the folder of the player's files once created: the
 * player only. */
constexpr std::filesystem::perms PLAYER_FOLDER_PERMISSIONS =
    std::filesystem::perms::owner_all;

}  // namespace rtype::client
