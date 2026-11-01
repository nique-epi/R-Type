#pragma once

#include "ConnectionPhase.hpp"
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

/**
 * @returns What tells the player the session with the server ended, for a
 * DialogScreen over the title: RETRY is selected at first, and closes the
 * dialog to open the connection screen over the title, its fields those of the
 * lost game. OK, which MENU_CANCEL_KEY answers too, closes the dialog and
 * leaves the title.
 *
 * @param navigator Screens around the dialog, which must outlive it.
 * @param sounds Plays the click of its buttons, and must outlive it.
 * @param ending How the session ended: Closed when the server said it shut
 * down; any other phase reads as a server that went silent.
 */
[[nodiscard]] Menu connectionLostMenu(IScreenNavigator& navigator,
                                      ISoundPlayer& sounds,
                                      ConnectionPhase ending);

}  // namespace rtype::client
