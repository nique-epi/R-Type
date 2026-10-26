#pragma once

#include "AssetList.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief The assets of the interface, which any screen may use at any time,
 * in a game or outside one: the two fonts and every sound of
 * SoundConstants.hpp, read once at launch and never released. */
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

/** @brief The assets of the intro: those of the title screen, which comes
 * after it, and the effect sheet of its explosions. */
[[nodiscard]] AssetList introAssets();

/**
 * @brief Loads the assets of the intro at once, so its first frame already
 * shows it; they are not permanent, and the title screen releases the effect
 * sheet when it opens. Call it after loadInterfaceAssets(), whose assets stay
 * loaded.
 *
 * @throws AssetLoadingException naming every asset of the intro that could not
 * be loaded.
 */
void loadIntroAssets(AssetLibrary& library);

}  // namespace rtype::client
