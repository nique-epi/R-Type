#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace rtype::server {

/**
 * @returns The leaderboard file of the server on @p port: @p setting, the
 * value of its database setting, or, while that is empty,
 * DATABASE_FILE_PREFIX, @p port then DATABASE_FILE_SUFFIX in @p folder.
 */
[[nodiscard]] std::filesystem::path databaseFileOf(
    std::string_view setting, const std::filesystem::path& folder,
    std::uint16_t port);

}  // namespace rtype::server
