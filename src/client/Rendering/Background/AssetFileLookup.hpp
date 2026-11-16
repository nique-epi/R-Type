#pragma once

#include <filesystem>
#include <functional>
#include <string_view>

namespace rtype::client {

/** @brief Finds the file an asset id names in the assets folder. */
using AssetFileLookup = std::function<std::filesystem::path(std::string_view)>;

}  // namespace rtype::client
