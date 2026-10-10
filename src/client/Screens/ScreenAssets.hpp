#pragma once

#include "AssetList.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief The assets of the interface, which any screen may use at any time,
 * in a game or outside one: read once at launch and never released. */
[[nodiscard]] AssetList interfaceAssets();

/** @brief The assets a game needs, loaded by the loading screen when the
 * player enters it. */
[[nodiscard]] AssetList gameAssets();

/**
 * @brief Loads the interface assets, then makes them permanent.
 *
 * @throws AssetLoadingException naming every interface asset that could not
 * be loaded.
 */
void loadInterfaceAssets(AssetLibrary& library);

}  // namespace rtype::client
