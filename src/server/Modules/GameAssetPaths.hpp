#pragma once

#include <filesystem>
#include <span>
#include <vector>

namespace rtype::server {

/** @returns The waves files the games play: WAVE_FILE_IN_ASSETS of each of
 * @p contentFolders that holds one, in their order (see
 * engine::contentFilesOf()), so a resource lays its waves over those of the
 * assets. */
[[nodiscard]] std::vector<std::filesystem::path> wavesFilesOf(
    std::span<const std::filesystem::path> contentFolders);

}  // namespace rtype::server
