#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The screen the client opens on, entered through replaceAll() only,
 * at launch: it replaces itself with TITLE_AFTER_INTRO_SCREEN. */
constexpr ScreenId INTRO_SCREEN{"intro"};

/** @brief The title screen, entered through replaceAll() only: building it
 * releases the assets the title does not use. */
constexpr ScreenId TITLE_SCREEN{"title"};

/** @brief The title screen the intro gives way to: its squadron starts in
 * place, where the ships of the intro landed. */
constexpr ScreenId TITLE_AFTER_INTRO_SCREEN{"title after the intro"};

/** @brief The options, pushed over the title, which they hide. */
constexpr ScreenId OPTIONS_SCREEN{"options"};

/** @brief The options, pushed over a game, which goes on behind them,
 * dimmed. */
constexpr ScreenId GAME_OPTIONS_SCREEN{"game options"};

/** @brief The dialog that says the session with the server ended, lost or
 * closed, which ClientScreens pushes over the title it puts in place of every
 * screen; RETRY opens the connection screen over the title, OK leaves the
 * title. */
constexpr ScreenId CONNECTION_LOST_SCREEN{"connection lost"};

/** @brief The question asked before closing the client, pushed over the
 * title. */
constexpr ScreenId QUIT_CONFIRMATION_SCREEN{"quit confirmation"};

/** @brief The server to join and the nickname to join it with, then how the
 * attempt stands, pushed over the title. */
constexpr ScreenId CONNECTION_SCREEN{"connection"};

/** @brief The same screen, pushed over the leaderboard: once the server
 * accepted the player, it closes, back to the leaderboard. */
constexpr ScreenId CONNECTION_FOR_LEADERBOARD_SCREEN{
    "connection for the leaderboard"};

/** @brief The best scores of a server, pushed over the title or the waiting
 * room. */
constexpr ScreenId LEADERBOARD_SCREEN{"leaderboard"};

}  // namespace rtype::client
