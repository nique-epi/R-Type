#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace rtype::client {

/**
 * @brief The file the token of the session is kept in, in the settings
 * folder, and the largest file read, in bytes (a token of the master holds
 * 67).
 */
constexpr std::string_view SESSION_TOKEN_FILE_NAME = "session";
constexpr std::uintmax_t MAXIMUM_SESSION_TOKEN_FILE_SIZE = 1024;

/** @brief Who may read and write the session file: the player only. */
constexpr std::filesystem::perms SESSION_TOKEN_FILE_PERMISSIONS =
    std::filesystem::perms::owner_read | std::filesystem::perms::owner_write;

/** @brief Logger name the account module writes its log lines under. */
constexpr std::string_view ACCOUNT_LOGGER_NAME = "Account";

}  // namespace rtype::client
