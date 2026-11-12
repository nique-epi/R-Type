#pragma once

#include <filesystem>
#include <optional>
#include "ReleaseNotes.hpp"

namespace rtype::client {

/** @brief Writes @p notes to @p file, as a JSON object, its folder made first.
 * @returns Whether every byte was written. */
[[nodiscard]] bool writeReleaseNotes(const std::filesystem::path& file,
                                     const ReleaseNotes& notes);

/** @returns The notes @p file holds, or nothing when it is missing or is not
 * what writeReleaseNotes() writes. */
[[nodiscard]] std::optional<ReleaseNotes> readReleaseNotes(
    const std::filesystem::path& file);

}  // namespace rtype::client
