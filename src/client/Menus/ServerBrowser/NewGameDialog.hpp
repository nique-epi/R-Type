#pragma once

#include "Menu.hpp"
#include "NewGameMenuParts.hpp"

namespace rtype::client {

/**
 * @returns The dialog that sets the rules of a new game, for a DialogScreen
 * over the server map: a row for the difficulty, among those of the server or
 * DEFAULT_DIFFICULTY alone when it listed none; a row for the lives of every
 * pilot, LOWEST_LIVES to HIGHEST_LIVES; a switch for friendly fire. It opens
 * on the default rules, at the first difficulty of the server when it has no
 * DEFAULT_DIFFICULTY. CREATE closes the dialog, then opens the connection to
 * the server for a NewGame with those rules, as joining a game of the map does
 * (openConnectionTo): BACK from the connection screen comes back to the map,
 * after a refusal too, never to the dialog. BACK, which MENU_CANCEL_KEY answers
 * too, closes it.
 */
[[nodiscard]] Menu newGameMenu(NewGameMenuParts parts);

}  // namespace rtype::client
