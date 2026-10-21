#pragma once

#include "AssetList.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief The assets of the interface, which any screen may use at any time,
 * in a game or outside one: read once at launch and never released. */
[[nodiscard]] AssetList interfaceAssets();

/** @brief The assets of the title screen: its logo, and the entity sheet its
 * squadron is drawn from. */
[[nodiscard]] AssetList titleAssets();

/** @brief The assets a game needs, loaded by the loading screen when the
 * player enters it: the entity sheet, the effect sheet, and the HUD sheet and
 * the plug icon of the status band. */
[[nodiscard]] AssetList gameAssets();

/**
 * @brief Loads the interface assets, then makes them permanent.
 *
 * @throws AssetLoadingException naming every interface asset that could not
 * be loaded.
 */
void loadInterfaceAssets(AssetLibrary& library);

/**
 * @brief Loads the assets of the title screen at once, so the first frame
 * already shows it; they are not permanent. Call it after
 * loadInterfaceAssets(), whose assets stay loaded.
 *
 * @throws AssetLoadingException naming every title asset that could not be
 * loaded.
 */
void loadTitleAssets(AssetLibrary& library);

}  // namespace rtype::client
