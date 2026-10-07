#pragma once

#include "AssetList.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief The assets any screen may use at any time, read once at launch and
 * never released. */
[[nodiscard]] AssetList interfaceAssets();

/** @brief The assets a game needs, loaded when the player enters it. */
[[nodiscard]] AssetList gameAssets();

/** @brief Loads the interface assets and makes them permanent. */
void loadInterfaceAssets(AssetLibrary& library);

}  // namespace rtype::client
