#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/** @brief Where the releases of the game are published: the feeds of the
 * latest one at LATEST_RELEASE_PATH and the feed name; the packages of a
 * release at RELEASE_DOWNLOAD_PATH, its version, '/' and the file name. */
constexpr std::string_view RELEASES_ORIGIN = "https://github.com";
constexpr std::string_view LATEST_RELEASE_PATH =
    "/nique-epi/R-Type/releases/latest/download/";
constexpr std::string_view RELEASE_DOWNLOAD_PATH =
    "/nique-epi/R-Type/releases/download/v";
constexpr char RELEASE_PATH_SEPARATOR = '/';

/** @brief How long a request waits to connect, then for each read. */
constexpr std::chrono::seconds RELEASE_REQUEST_TIMEOUT{5};

/** @brief The largest feed read: one release of one system is about 1 KB, so
 * this leaves room for many releases. */
constexpr std::size_t MAXIMUM_FEED_BYTES = 262144;
/** @brief The largest package downloaded; a client package weighs about
 * 40 MB. */
constexpr std::uint64_t MAXIMUM_PACKAGE_BYTES = 536870912;
/** @brief The longest name of a feed or of a package, which holds only
 * letters, digits and FILE_NAME_PUNCTUATION and never starts with a dot. */
constexpr std::size_t MAXIMUM_FILE_NAME_SIZE = 128;
constexpr std::string_view FILE_NAME_PUNCTUATION = ".-_";
constexpr char FILE_NAME_DOT = '.';

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
