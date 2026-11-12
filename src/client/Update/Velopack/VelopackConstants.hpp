#pragma once

#include <string_view>

namespace rtype::client {

/** @brief The steps an UpdateException names. */
constexpr std::string_view LOCATE_STEP = "find the installed client";
constexpr std::string_view CHECK_STEP = "check the latest release";
constexpr std::string_view DOWNLOAD_STEP = "download the release";
constexpr std::string_view APPLY_STEP = "start the updater";
/** @brief The updater of Velopack shows no window of its own, the panel of
 * the client says what happens, and it starts the new version once done. */
constexpr bool SILENT_UPDATER = true;
constexpr bool RESTART_AFTER_UPDATE = true;

/** @brief What download() answers when no release was found before. */
constexpr std::string_view NO_RELEASE_FOUND = "no newer release was found";

}  // namespace rtype::client
