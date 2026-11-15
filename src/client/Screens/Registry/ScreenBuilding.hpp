#pragma once

#include <memory>
#include "AssetList.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "ISoundPlayer.hpp"
#include "InterfaceLook.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/** @returns Reduced when the system asks for less motion, Full otherwise;
 * read each time a screen that moves is built. */
[[nodiscard]] MotionPreference motionPreference();

/** @returns The player of the short sounds of the audio service. */
[[nodiscard]] ISoundPlayer& soundsOf(const ScreenContext& context);

/** @returns The look of the interface the settings ask for: the text size of
 * TEXT SIZE and whether HIGH CONTRAST is on. */
[[nodiscard]] InterfaceLook interfaceLookOf(const PlayerSettings& settings);

/** @returns A new picture for a menu, written with the fonts of the asset
 * library, in the look the settings of the context ask for at each frame. */
[[nodiscard]] std::unique_ptr<IInterfaceCanvas> interfaceCanvasOf(
    const ScreenContext& context);

/** @returns The point of the playfield under a pixel of the window. */
[[nodiscard]] PlayfieldMapping playfieldMappingOf(const ScreenContext& context);

/** @returns A dialog screen that shows @p menu over the screens below. */
[[nodiscard]] std::unique_ptr<IScreen> dialogScreenOf(
    const ScreenContext& context, Menu menu);

/**
 * @brief The loading screen of a list while some of its assets are not
 * loaded; nothing once they all are, after releasing the loaded assets the
 * list does not name, as the loading screen does.
 *
 * @param screen Screen the loading screen gives way to.
 */
[[nodiscard]] std::unique_ptr<IScreen> loadingScreenUnlessLoaded(
    const ScreenContext& context, const AssetList& assets, ScreenId screen);

}  // namespace rtype::client
