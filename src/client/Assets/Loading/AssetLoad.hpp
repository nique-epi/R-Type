#pragma once

#include <functional>

namespace rtype::client {

class AssetLibrary;

/** @brief A load of assets into the library, such as those the assets module
 * runs when it starts. */
using AssetLoad = std::function<void(AssetLibrary&)>;

}  // namespace rtype::client
