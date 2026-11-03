#pragma once

#include <string_view>

namespace rtype::client {

/** @brief Tells whether an asset id follows the asset id rule. */
[[nodiscard]] bool isValidAssetId(std::string_view assetId);

}  // namespace rtype::client
