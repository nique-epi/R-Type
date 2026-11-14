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
 * screen; RETRY opens the server map and CONNECTION_AGAIN_SCREEN over it, OK
 * leaves the title. */
constexpr ScreenId CONNECTION_LOST_SCREEN{"connection lost"};

/** @brief The question asked before closing the client, pushed over the
 * title. */
constexpr ScreenId QUIT_CONFIRMATION_SCREEN{"quit confirmation"};

/** @brief The question asked before leaving a game for the title, pushed by
 * QUIT over the options of the game. */
constexpr ScreenId LEAVE_GAME_CONFIRMATION_SCREEN{"leave game confirmation"};

/** @brief The server to join and the nickname to join it with, then how the
 * attempt stands, pushed by ADDRESS over the server map. */
constexpr ScreenId CONNECTION_SCREEN{"connection"};

/** @brief The same screen, pushed over the leaderboard: once the server
 * accepted the player, it closes, back to the leaderboard. */
constexpr ScreenId CONNECTION_FOR_LEADERBOARD_SCREEN{
    "connection for the leaderboard"};

/** @brief The same screen, which starts its attempt as it opens: pushed by
 * PLAY over the title for a client launched with `-h`. */
constexpr ScreenId CONNECTION_TO_CHOSEN_SERVER_SCREEN{
    "connection to the chosen server"};

/** @brief The same screen with the nickname alone, pushed over the server map
 * with the server and the game the map chose: the master listed the server,
 * so the player types no address. */
constexpr ScreenId CONNECTION_TO_MAP_SERVER_SCREEN{
    "connection to a server of the map"};

/** @brief The same screen, pushed by RETRY over the server map: its fields
 * start with the server of the lost game, and the player confirms them. */
constexpr ScreenId CONNECTION_AGAIN_SCREEN{"connection again"};

/** @brief The dialog that sets the rules of a new game, pushed over the
 * server map by + NEW GAME of the panel of the games of a server. */
constexpr ScreenId NEW_GAME_SCREEN{"new game"};

/** @brief Where PLAY leads, pushed over the title: SERVER, SHOP or HANGAR on
 * a turning stand. */
constexpr ScreenId DESTINATION_SCREEN{"destinations"};

/** @brief The map of the servers the master lists, with their games, pushed
 * over the destination screen by SERVER, or by RETRY under
 * CONNECTION_AGAIN_SCREEN. */
constexpr ScreenId SERVER_MAP_SCREEN{"server map"};

/** @brief The best scores of a server, pushed over the title or the waiting
 * room. */
constexpr ScreenId LEADERBOARD_SCREEN{"leaderboard"};

}  // namespace rtype::client
