#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The players of the server joined and whether each is ready, pushed
 * over the connection screen once the server accepted the player. */
constexpr ScreenId LOBBY_SCREEN{"lobby"};

/** @brief The waiting room after a game, entered through replaceAll() only:
 * alone on the stack, its BACK leaves the server for the title. */
constexpr ScreenId LOBBY_AFTER_GAME_SCREEN{"lobby after a game"};

}  // namespace rtype::client
