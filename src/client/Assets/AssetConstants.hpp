#pragma once

#include <string_view>

namespace rtype::client {

constexpr std::string_view ASSETS_FOLDER_NAME = "assets";

/** @brief Separates the folders of an asset id, on every platform. */
constexpr char ASSET_ID_SEPARATOR = '/';

/** @brief Characters an asset id segment may hold after its first one,
 * besides a-z and 0-9. */
constexpr std::string_view ASSET_ID_PUNCTUATION = "_-.";

}  // namespace rtype::client
