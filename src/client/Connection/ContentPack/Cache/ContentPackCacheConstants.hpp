#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

/** @brief The packs a client keeps: past it, the one used longest ago goes. */
constexpr std::size_t MAXIMUM_CACHED_PACKS = 8;

/** @brief The folder of the settings folder the packs are kept in, and the
 * extension of a kept pack. */
constexpr std::string_view PACK_CACHE_FOLDER_NAME = "content-packs";
constexpr std::string_view PACK_FILE_EXTENSION = ".pack";

/** @brief What a pack is written as before it is complete, so a pack cut by
 * a crash is never taken for a whole one. */
constexpr std::string_view PARTIAL_PACK_FILE_SUFFIX = ".part";

}  // namespace rtype::client
