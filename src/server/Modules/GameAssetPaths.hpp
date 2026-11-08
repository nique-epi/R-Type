#pragma once

#include <filesystem>
#include <span>

namespace rtype::server {

/** @returns The waves file the games play: WAVE_FILE_IN_ASSETS of the last of
 * @p contentFolders that holds one (see engine::topmostContentFile()), so a
 * resource replaces the waves of the assets. */
[[nodiscard]] std::filesystem::path wavesFileOf(
    std::span<const std::filesystem::path> contentFolders);

}  // namespace rtype::server
