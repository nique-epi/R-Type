#pragma once

#include <string_view>

namespace rtype::client {

/** @brief Tells whether an asset id follows the asset id rule. */
[[nodiscard]] bool isValidAssetId(std::string_view assetId);

/** @brief Tells whether an asset is named by the whole path of its file, as
 * the files of a content pack are, which lie outside the assets folder. */
[[nodiscard]] bool isWholeFilePath(std::string_view assetName);

}  // namespace rtype::client
