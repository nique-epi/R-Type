#pragma once

#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"

namespace rtype::client {

/**
 * @returns The question asked before closing the client, for a DialogScreen:
 * NO is selected at first, and MENU_CANCEL_KEY answers it too: both close the
 * dialog. YES closes every screen, which closes the client.
 *
 * @param navigator Screens around the dialog, which must outlive it.
 * @param sounds Plays the click of its buttons, and must outlive it.
 */
[[nodiscard]] Menu quitConfirmationMenu(IScreenNavigator& navigator,
                                        ISoundPlayer& sounds);

}  // namespace rtype::client
