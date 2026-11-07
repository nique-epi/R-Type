#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/** @brief The tries a download of a content pack makes: the first, then two
 * that resume where the last one was cut. */
constexpr std::size_t PACK_DOWNLOAD_ATTEMPTS = 3;

/** @brief How long a download waits to connect, then for each read or write,
 * before it counts the try as cut. */
constexpr std::chrono::seconds PACK_CONNECT_TIMEOUT{5};
constexpr std::chrono::seconds PACK_TRANSFER_TIMEOUT{5};

/** @brief The packs a client keeps: past it, the one used longest ago goes. */
constexpr std::size_t MAXIMUM_CACHED_PACKS = 8;

/** @brief The folder of the settings folder the packs are kept in, and the
 * extension of a kept pack. */
constexpr std::string_view PACK_CACHE_FOLDER_NAME = "content-packs";
constexpr std::string_view PACK_FILE_EXTENSION = ".pack";

/** @brief What a pack is written as before it is complete, so a pack cut by
 * a crash is never taken for a whole one. */
constexpr std::string_view PARTIAL_PACK_FILE_SUFFIX = ".part";

/** @brief What a range of bytes from an offset to the end of a body ends with
 * (cpp-httplib). */
constexpr std::int64_t RANGE_TO_THE_END = -1;

}  // namespace rtype::client
