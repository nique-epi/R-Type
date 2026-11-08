#pragma once

#include <array>
#include "GameScreenIdConstants.hpp"
#include "LobbyScreenIdConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ScreenId.hpp"

/** @brief The screens the menus module registers. */
constexpr std::array<rtype::client::ScreenId, 13> MENU_SCREENS{
    rtype::client::INTRO_SCREEN,
    rtype::client::TITLE_SCREEN,
    rtype::client::TITLE_AFTER_INTRO_SCREEN,
    rtype::client::OPTIONS_SCREEN,
    rtype::client::GAME_OPTIONS_SCREEN,
    rtype::client::QUIT_CONFIRMATION_SCREEN,
    rtype::client::CONNECTION_SCREEN,
    rtype::client::CONNECTION_FOR_LEADERBOARD_SCREEN,
    rtype::client::CONNECTION_TO_CHOSEN_SERVER_SCREEN,
    rtype::client::CONNECTION_AGAIN_SCREEN,
    rtype::client::CONNECTION_LOST_SCREEN,
    rtype::client::SERVER_MAP_SCREEN,
    rtype::client::LEADERBOARD_SCREEN};

/** @brief The screens the lobby module registers. */
constexpr std::array<rtype::client::ScreenId, 2> LOBBY_SCREENS{
    rtype::client::LOBBY_SCREEN, rtype::client::LOBBY_AFTER_GAME_SCREEN};

/** @brief The screens the game module registers. */
constexpr std::array<rtype::client::ScreenId, 2> GAME_SCREENS{
    rtype::client::GAME_SCREEN, rtype::client::END_SCREEN};
