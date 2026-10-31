#pragma once

#include "AssetList.hpp"

namespace rtype::client {

class AssetLibrary;

/** @brief The assets of the interface, which any screen may use at any time,
 * in a game or outside one: the two fonts and every sound of
 * SoundConstants.hpp, read once at launch and never released. */
[[nodiscard]] AssetList interfaceAssets();

/** @brief The assets of the title screen: its logo, and the entity sheet its
 * squadron is drawn from; with those of the screens pushed over it, so the
 * waiting room opens without loading: the HUD sheet and the professor sheet
 * its help draws. */
[[nodiscard]] AssetList titleAssets();

/** @brief The assets a game needs, loaded by the loading screen when the
 * player enters it: those of the title, kept whole so a title built after a
 * game opens without a loading screen, which would close what is pushed over it
 * when it gives way; then the effect sheet, the plug icon of the status band,
 * and the sheet of the flight log, so the end screen opens without loading; the
 * end screen keeps them all, so the next game opens without loading too. */
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
