#pragma once

#include <memory>
#include <optional>
#include "AssetList.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"

namespace rtype::client {

/** @returns Reduced when the system asks for less motion, Full otherwise;
 * read each time a screen that moves is built. */
[[nodiscard]] MotionPreference motionPreference();

/** @returns The player of the short sounds of the audio service. */
[[nodiscard]] ISoundPlayer& soundsOf(const ScreenContext& context);

/** @returns A new picture for a menu, written with the fonts of the asset
 * library. */
[[nodiscard]] std::unique_ptr<IInterfaceCanvas> interfaceCanvasOf(
    const ScreenContext& context);

/** @returns The point of the playfield under a pixel of the window. */
[[nodiscard]] PlayfieldMapping playfieldMappingOf(const ScreenContext& context);

/** @returns A dialog screen that shows @p menu over the screens below. */
[[nodiscard]] std::unique_ptr<IScreen> dialogScreenOf(
    const ScreenContext& context, Menu menu);

/** @returns The dialog that tells a guest a sign-in is needed: SIGN IN pushes
 * SIGN_IN_SCREEN over it, which closes it once signed in and opens @p asking
 * in its place when it names a screen; OK, which MENU_CANCEL_KEY answers too,
 * closes it. */
[[nodiscard]] std::unique_ptr<IScreen> signInNeededScreenOf(
    const ScreenContext& context, std::optional<ScreenId> asking);

/** @returns A builder of what @p builder builds for a player signed in, and
 * of the dialog that tells a guest a sign-in is needed
 * (signInNeededScreenOf()) in its place, @p asking being the screen it is
 * registered as. */
[[nodiscard]] ScreenRegistry::Builder signedInOnly(
    ScreenRegistry::Builder builder, ScreenId asking);

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
