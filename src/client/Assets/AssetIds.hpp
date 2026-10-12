#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace rtype::client {

/** @brief Tells whether an asset id follows the asset id rule. */
[[nodiscard]] bool isValidAssetId(std::string_view assetId);

/** @brief The UTF-8 text of a path, with '/' between folders on every
 * platform. */
[[nodiscard]] std::string genericText(const std::filesystem::path& path);

}  // namespace rtype::client
