#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The game in progress, entered through replaceAll() only: building
 * it releases the assets of a previous game. */
constexpr ScreenId GAME_SCREEN{"game"};

/** @brief The end of a game: its scores in the flight log, then PRESS ENTER
 * TO REPLAY or QUIT; entered through replaceAll() only, by the game once the
 * server ended it. */
constexpr ScreenId END_SCREEN{"end"};

}  // namespace rtype::client
