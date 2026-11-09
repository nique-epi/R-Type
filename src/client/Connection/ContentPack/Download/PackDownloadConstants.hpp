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

/** @brief What a range of bytes from an offset to the end of a body ends with
 * (cpp-httplib). */
constexpr std::int64_t RANGE_TO_THE_END = -1;

/** @brief Scheme the origin of a game server starts with: packs travel in
 * plain HTTP, their fingerprint checked once received. */
constexpr std::string_view HTTP_ORIGIN_PREFIX = "http://";

}  // namespace rtype::client
