#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "LetterCounts.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Logger name the game screen writes its log lines under. */
constexpr std::string_view GAME_SCREEN_LOGGER_NAME = "GameScreen";

/** @brief Key whose release opens the options during a game: the key that
 * closes every menu, so the same key opens and closes them. */
constexpr sf::Keyboard::Key OPTIONS_KEY = MENU_CANCEL_KEY;

/** @brief Time the game goes on after the server ended it before the end
 * screen replaces it, so the last explosion plays. */
constexpr engine::Duration END_SCREEN_DELAY = std::chrono::milliseconds(1000);

/** @brief Asset id of the logo of the title screen, and its size in
 * pixels. */
constexpr const char* LOGO_ID = "sprites/logo.png";
constexpr sf::Vector2i LOGO_SIZE(210, 42);

/** @brief Logger name the intro writes its log line under. */
constexpr std::string_view INTRO_LOGGER_NAME = "Intro";

/**
 * @brief Texts of the title screen. The copyright sign is written as its two
 * UTF-8 bytes, so every compiler stores the same text. The version line is
 * GAME_VERSION_TEXT, which CMake writes from project() into
 * ClientVersionConstants.hpp.
 */
constexpr Translation TITLE_SUBTITLE{.english = "4-PLAYER ONLINE CO-OP",
                                     .french = "COOP EN LIGNE À 4"};
constexpr Translation PLAY_LABEL{.english = "PLAY", .french = "JOUER"};
constexpr Translation PLAY_DESCRIPTION{
    .english = "> JOIN A SERVER, FIGHT",
    .french = "> REJOINDRE UN SERVEUR, COMBATTRE"};
constexpr Translation SETTINGS_LABEL{.english = "SETTINGS",
                                     .french = "OPTIONS"};
constexpr Translation SETTINGS_DESCRIPTION{.english = "> SOUND, KEYS, ACCESS",
                                           .french = "> SON, TOUCHES, ACCÈS"};
constexpr Translation QUIT_LABEL{.english = "QUIT", .french = "QUITTER"};
constexpr Translation QUIT_DESCRIPTION{.english = "> LEAVE THE GAME",
                                       .french = "> QUITTER LE JEU"};
/** @brief Label of the entry on the title and in the waiting room, title of the
 * screen it opens, and the line under the bar of the title. */
constexpr Translation LEADERBOARD_LABEL{.english = "RANKING",
                                        .french = "CLASSEMENT"};
constexpr Translation LEADERBOARD_DESCRIPTION{
    .english = "> THE BEST SCORES OF A SERVER",
    .french = "> LES MEILLEURS SCORES D'UN SERVEUR"};
constexpr const char* COPYRIGHT_TEXT = "\xC2\xA9 2026 R-TYPE TEAM";

/** @brief Texts of the quit confirmation. */
constexpr Translation CONFIRM_HEADING{.english = "CONFIRM",
                                      .french = "CONFIRMER"};
constexpr Translation QUIT_QUESTION{.english = "ARE YOU SURE YOU WANT TO QUIT?",
                                    .french = "QUITTER POUR DE BON ?"};
constexpr Translation NO_LABEL{.english = "NO", .french = "NON"};
constexpr Translation YES_LABEL{.english = "YES", .french = "OUI"};

/** @brief Texts of the dialog shown once the session with the server ended. */
constexpr Translation CONNECTION_LOST_HEADING{.english = "CONNECTION LOST",
                                              .french = "CONNEXION PERDUE"};
constexpr Translation SILENT_SERVER_MESSAGE{
    .english = "THE SERVER STOPPED ANSWERING.",
    .french = "LE SERVEUR NE RÉPOND PLUS."};
constexpr Translation CLOSED_SERVER_MESSAGE{
    .english = "THE SERVER SHUT DOWN.", .french = "LE SERVEUR S'EST ARRÊTÉ."};
constexpr Translation EXPELLED_MESSAGE{.english = "THE HOST EXPELLED YOU.",
                                       .french = "L'HÔTE VOUS A EXPULSÉ."};
constexpr Translation RETRY_LABEL{.english = "RETRY", .french = "RÉESSAYER"};
constexpr Translation OK_LABEL{.english = "OK", .french = "OK"};

/** @brief Texts of the dialog that tells a guest the kopecks, the chips and
 * the looks live on an account of the master. */
constexpr Translation SIGN_IN_HEADING{.english = "SIGN IN NEEDED",
                                      .french = "CONNEXION REQUISE"};
constexpr Translation SIGN_IN_MESSAGE{
    .english = "SIGN IN TO SPEND KOPECKS.",
    .french = "SE CONNECTER POUR LES KOPECKS."};
/** @brief The button of that dialog that opens SIGN_IN_SCREEN. */
constexpr Translation SIGN_IN_LABEL{.english = "SIGN IN",
                                    .french = "CONNEXION"};

/** @brief The letters of the longest message of a dialog, the English
 * question of QUIT, which the dialog shows whole: every other message, in
 * every language, holds no more. */
constexpr std::size_t LONGEST_DIALOG_MESSAGE_LETTERS =
    lettersIn(QUIT_QUESTION.english);
static_assert(longestLettersIn(QUIT_QUESTION) <= LONGEST_DIALOG_MESSAGE_LETTERS,
              "the question of QUIT fits its dialog");
static_assert(longestLettersIn(SILENT_SERVER_MESSAGE) <=
                  LONGEST_DIALOG_MESSAGE_LETTERS,
              "a silent server is told in one line");
static_assert(longestLettersIn(CLOSED_SERVER_MESSAGE) <=
                  LONGEST_DIALOG_MESSAGE_LETTERS,
              "a closed server is told in one line");
static_assert(longestLettersIn(EXPELLED_MESSAGE) <=
                  LONGEST_DIALOG_MESSAGE_LETTERS,
              "an expulsion is told in one line");
static_assert(longestLettersIn(SIGN_IN_MESSAGE) <=
                  LONGEST_DIALOG_MESSAGE_LETTERS,
              "the sign in needed is told in one line");

/**
 * @brief The word after a number of pilots, as the waiting room counts its
 * players and the leaderboard names a number of players at the start.
 */
constexpr Translation ONE_PILOT_TEXT{.english = " PILOT", .french = " PILOTE"};
constexpr Translation SEVERAL_PILOTS_TEXT{.english = " PILOTS",
                                          .french = " PILOTES"};

/**
 * @brief Size of the progress bar of the loading screen, centered in the
 * playfield, and thickness of its border, in playfield units.
 */
constexpr float LOADING_BAR_WIDTH = 960.0F;
constexpr float LOADING_BAR_HEIGHT = 24.0F;
constexpr float LOADING_BAR_OUTLINE_THICKNESS = 3.0F;

}  // namespace rtype::client
