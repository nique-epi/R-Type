#pragma once

#include "ConnectionPhase.hpp"
#include "DialogMaking.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"

namespace rtype::client {

/**
 * @returns The question asked before closing the client, for a DialogScreen:
 * NO is selected at first, and MENU_CANCEL_KEY answers it too: both close the
 * dialog. YES closes every screen, which closes the client.
 *
 * @param making The screens around it, the player of its clicks and its
 * text size.
 */
[[nodiscard]] Menu quitConfirmationMenu(const DialogMaking& making);

/**
 * @returns The question asked before leaving a game for the title, for a
 * DialogScreen over the options of the game: NO is selected at first, and
 * MENU_CANCEL_KEY answers it too: both close the dialog, back to the options.
 * YES leaves the session, which tells the server, and puts the title in place
 * of every screen.
 *
 * @param making The screens around it, the player of its clicks and its
 * text size.
 * @param connection The session left, which must outlive the dialog.
 */
[[nodiscard]] Menu leaveGameConfirmationMenu(const DialogMaking& making,
                                             IServerConnection& connection);

/**
 * @returns What tells the player the session with the server ended, for a
 * DialogScreen over the title: RETRY is selected at first, and closes the
 * dialog to open the server map over the title and CONNECTION_AGAIN_SCREEN over
 * the map, its fields those of the lost game, so BACK leads to the map. OK,
 * which MENU_CANCEL_KEY answers too, closes the dialog and leaves the title.
 *
 * @param making The screens around it, the player of its clicks and its
 * text size.
 * @param ending How the session ended: Closed when the server said it shut
 * down; any other phase reads as a server that went silent.
 */
[[nodiscard]] Menu connectionLostMenu(const DialogMaking& making,
                                      ConnectionPhase ending);

}  // namespace rtype::client
