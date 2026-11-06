#pragma once

#include <filesystem>
#include <optional>

namespace rtype::client {

/**
 * @brief Folder where the system keeps the logs of the user's programs, where
 * the client writes its default journal, or nothing when the journal goes next
 * to the executable.
 *
 * On macOS, the Logs folder of the user's Library, which always exists and
 * which Console shows, so the journal never writes into the signed app; on
 * Linux and Windows, nothing.
 */
[[nodiscard]] std::optional<std::filesystem::path> journalFolder();

}  // namespace rtype::client
