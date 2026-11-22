#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "GameRuleConstants.hpp"
#include "InterfaceConstants.hpp"
#include "ItemListColumn.hpp"
#include "ItemListLayout.hpp"
#include "LeaderboardFigure.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardMessage.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The tabs of the screen. */
constexpr Translation SERVER_TAB_LABEL{.english = "SERVER",
                                       .french = "SERVEUR"};
constexpr Translation GLOBAL_TAB_LABEL{.english = "GLOBAL",
                                       .french = "MONDIAL"};

/** @brief The chips of the screen and the bar under its message. */
constexpr Translation FILTER_LABEL{.english = "FILTER", .french = "FILTRE"};
constexpr sf::Keyboard::Key FILTER_KEY = sf::Keyboard::Key::F;
constexpr const char* FILTER_KEY_NAME = "F";
constexpr Translation JOIN_LABEL{.english = "JOIN A SERVER",
                                 .french = "REJOINDRE UN SERVEUR"};

/** @brief Texts of the filter dialog. */
constexpr Translation PILOTS_ROW_LABEL{.english = "PILOTS",
                                       .french = "PILOTES"};
constexpr Translation APPLY_LABEL{.english = "APPLY", .french = "APPLIQUER"};

/** @brief The label of the choice that asks for every value of a row: every
 * difficulty, every number of pilots. */
constexpr Translation ALL_DIFFICULTIES_LABEL{.english = "ALL",
                                             .french = "TOUTES"};
constexpr Translation ALL_PILOTS_LABEL{.english = "ALL", .french = "TOUS"};

/** @brief The difficulties a player can pick: ALL, then the difficulties every
 * server ships, as the waves file names them, in the order of their labels. */
constexpr std::array<std::string_view, 4> LEADERBOARD_DIFFICULTY_NAMES{
    network::ALL_DIFFICULTIES, identifiers::DIFFICULTY_ORDER[0],
    identifiers::DIFFICULTY_ORDER[1], identifiers::DIFFICULTY_ORDER[2]};
constexpr std::array<Translation, 4> LEADERBOARD_DIFFICULTY_LABELS{
    ALL_DIFFICULTIES_LABEL, Translation{.english = "EASY", .french = "FACILE"},
    Translation{.english = "NORMAL", .french = "NORMAL"},
    Translation{.english = "HARD", .french = "DIFFICILE"}};
static_assert(LEADERBOARD_DIFFICULTY_NAMES.size() ==
                  identifiers::DIFFICULTY_ORDER.size() + 1,
              "the filter offers ALL and every difficulty a server ships");

/** @brief The numbers of pilots a player can pick: ALL, then each number of
 * players of a game, so the place of a label is the number it asks for,
 * ALL_PLAYER_COUNTS for ALL. */
constexpr std::array<Translation, 5> LEADERBOARD_PILOT_LABELS{
    ALL_PILOTS_LABEL, Translation{.english = "1", .french = "1"},
    Translation{.english = "2", .french = "2"},
    Translation{.english = "3", .french = "3"},
    Translation{.english = "4", .french = "4"}};
static_assert(LEADERBOARD_PILOT_LABELS.size() == network::MAXIMUM_PLAYERS + 1,
              "the filter offers ALL and every number of players of a game");
static_assert(network::ALL_PLAYER_COUNTS == 0,
              "ALL is the first label of the pilots");

/** @brief The filter every leaderboard opens on: every difficulty and every
 * number of pilots. */
constexpr LeaderboardFilterChoice OPENING_LEADERBOARD_CHOICE{.difficulty = 0,
                                                             .pilots = 0};

/** @brief The summary under the panel: the difficulty, then the number of
 * pilots and the word after it. */
constexpr std::string_view DIFFICULTY_MARK = "{DIFFICULTY}";
constexpr std::string_view PILOT_COUNT_MARK = "{PILOTS}";
constexpr Translation FILTER_SUMMARY{
    .english = "{DIFFICULTY} GAMES, {PILOTS}",
    .french = "PARTIES : {DIFFICULTY}, {PILOTS}"};

/** @brief The court around the panel, as on version A V3 of the design: the
 * banners against the top of the picture, out of step; the guards beside the
 * title, out of step. */
constexpr std::array LEADERBOARD_FIGURES{
    LeaderboardFigure{.animation = &FLAG_ANIMATION, .topLeft = {8, 0}},
    LeaderboardFigure{.animation = &FLAG_ANIMATION,
                      .topLeft = {420, 0},
                      .lead = std::chrono::milliseconds(650)},
    LeaderboardFigure{.animation = &GUARD_FACING_LEFT_ANIMATION,
                      .topLeft = {146, 8}},
    LeaderboardFigure{.animation = &GUARD_FACING_RIGHT_ANIMATION,
                      .topLeft = {306, 8},
                      .lead = std::chrono::milliseconds(125)}};

/** @brief The screen has nothing new to show more often than its banners move:
 * it repaints this often, and on an event or a new answer. */
constexpr engine::Duration LEADERBOARD_REPAINT_TIME = FLAG_FRAME_TIME;

/** @brief The title, centered, twice its size. */
constexpr sf::Vector2i LEADERBOARD_TITLE_ANCHOR(240, 18);
constexpr TextStyle LEADERBOARD_TITLE_STYLE{.font = TextFont::Regular,
                                            .color = FIRE_GOLD,
                                            .outlined = true,
                                            .alignment = TextAlignment::Center,
                                            .scale = 2};

/** @brief The rule under the title, the tabs, and the panel under them. */
constexpr sf::IntRect LEADERBOARD_PANEL({80, 66}, {320, 150});
constexpr sf::IntRect LEADERBOARD_RULE({LEADERBOARD_PANEL.position.x, 40},
                                       {LEADERBOARD_PANEL.size.x, 1});
constexpr sf::Vector2i LEADERBOARD_TABS_TOP_LEFT(LEADERBOARD_PANEL.position.x,
                                                 50);
constexpr int LEADERBOARD_TABS_WIDTH = LEADERBOARD_PANEL.size.x;

/** @brief The columns of the list: the rank ends at 24; the pilot starts at 28
 * and the score ends at 240, so the longest nickname and YOU stop before a
 * score of 7 digits; the date ends with the row. */
constexpr std::array LEADERBOARD_COLUMNS{
    ItemListColumn{.anchor = 24, .alignment = TextAlignment::Right},
    ItemListColumn{.anchor = 28},
    ItemListColumn{.anchor = 240, .alignment = TextAlignment::Right},
    ItemListColumn{.anchor = 296,
                   .font = TextFont::Small,
                   .alignment = TextAlignment::Right}};

/** @brief The list in the panel, its 13 pixels between two rows reading better
 * than the 10 of the first versions of the design. It holds every row an answer
 * lists, so the last of them is painted; ten rows end at y 203, in a panel that
 * ends at 216: a protocol that lists more must be measured against the panel.
 */
constexpr ItemListLayout LEADERBOARD_LIST{
    .topLeft = {92, 73},
    .width = 296,
    .rowPitch = 13,
    .rowCapacity = network::MAXIMUM_LEADERBOARD_ROWS,
    .columns = LEADERBOARD_COLUMNS};

/** @brief Colors of a row; bronze is the one color the leaderboard adds to the
 * palette, 5.2:1 on the sky. */
constexpr sf::Color LEADERBOARD_BRONZE(194, 112, 46);
constexpr std::array<sf::Color, 3> LEADERBOARD_TOP_RANK_COLORS{
    FIRE_GOLD, STAR_WHITE, LEADERBOARD_BRONZE};
constexpr sf::Color LEADERBOARD_RANK_COLOR = PALE_BLUE;
constexpr sf::Color LEADERBOARD_TEXT_COLOR = STAR_WHITE;
constexpr sf::Color LEADERBOARD_DATE_COLOR = PALE_BLUE;
constexpr sf::Color LEADERBOARD_OWN_TEXT_COLOR = FIRE_YELLOW;
constexpr sf::Color LEADERBOARD_OWN_DATE_COLOR = STAR_WHITE;
constexpr sf::Color LEADERBOARD_OWN_ROW_COLOR = PRESSED_BAR_INSIDE_COLOR;

/** @brief The date of a row: the months, and what stands for a date past the
 * last one a year of two digits can tell, 31 DEC 99 at 23:59:59. */
constexpr std::array<Translation, 12> MONTH_NAMES{
    Translation{.english = "JAN", .french = "JAN"},
    Translation{.english = "FEB", .french = "FÉV"},
    Translation{.english = "MAR", .french = "MAR"},
    Translation{.english = "APR", .french = "AVR"},
    Translation{.english = "MAY", .french = "MAI"},
    Translation{.english = "JUN", .french = "JUN"},
    Translation{.english = "JUL", .french = "JUL"},
    Translation{.english = "AUG", .french = "AOÛ"},
    Translation{.english = "SEP", .french = "SEP"},
    Translation{.english = "OCT", .french = "OCT"},
    Translation{.english = "NOV", .french = "NOV"},
    Translation{.english = "DEC", .french = "DÉC"}};
constexpr std::int64_t LAST_SHOWN_DATE_SECONDS = 4102444799;
constexpr const char* UNKNOWN_DATE_TEXT = "-- --- --";
constexpr unsigned int FIRST_NUMBER_OF_TWO_DIGITS = 10;
constexpr int YEARS_PER_CENTURY = 100;

/** @brief What the panel says in place of rows. */
constexpr LeaderboardMessage EMPTY_LEADERBOARD_MESSAGE{
    .line = {.english = "NO SCORE IN THIS MODE YET",
             .french = "PAS ENCORE DE SCORE DANS CE MODE"},
    .detail = {.english = "BE THE FIRST TO FLY IT",
               .french = "SOYEZ LE PREMIER À LE JOUER"}};
constexpr LeaderboardMessage NOT_ON_SERVER_MESSAGE{
    .line = {.english = "NOT ON A SERVER", .french = "PAS SUR UN SERVEUR"},
    .detail = {.english = "JOIN ONE TO READ ITS RANKING",
               .french = "REJOIGNEZ-EN UN POUR SON CLASSEMENT"}};
constexpr LeaderboardMessage NO_ANSWER_MESSAGE{
    .line = {.english = "NO ANSWER FROM THE SERVER",
             .french = "PAS DE RÉPONSE DU SERVEUR"},
    .detail = {.english = "APPLY A FILTER TO ASK AGAIN",
               .french = "APPLIQUEZ UN FILTRE POUR REDEMANDER"}};
constexpr LeaderboardMessage OUT_OF_REACH_MESSAGE{
    .line = {.english = "THE MASTER IS OUT OF REACH",
             .french = "LE MAÎTRE EST INJOIGNABLE"},
    .detail = {.english = "THE SERVER TAB STILL WORKS",
               .french = "L'ONGLET SERVEUR MARCHE TOUJOURS"}};
/** @brief What the panel says while its source asks: a server or the master,
 * so it names neither. */
constexpr Translation ASKING_TEXT{.english = "ASKING", .french = "DEMANDE"};
constexpr Translation NO_OWN_SCORE_TEXT{
    .english = "YOU HAVE NO SCORE IN THIS MODE YET",
    .french = "VOUS N'AVEZ PAS DE SCORE DANS CE MODE"};

/** @brief A dot more after ASKING_TEXT each ASKING_DOT_TIME, up to three, then
 * none again. */
constexpr engine::Duration ASKING_DOT_TIME = std::chrono::milliseconds(300);
constexpr int ASKING_DOT_STEPS = 4;

/** @brief Where the panel writes them: its line and its small print, centered;
 * ASKING_TEXT from a fixed left in each language, so its dots never move it,
 * chosen so the text with its three dots is centered: in English 9
 * characters of 8 pixels from x 204, in French 10 from x 200. */
constexpr sf::Vector2i MESSAGE_LINE_ANCHOR(240, 130);
constexpr sf::Vector2i MESSAGE_DETAIL_ANCHOR(240, 144);
constexpr int ASKING_TOP = 138;
constexpr sf::Color MESSAGE_LINE_COLOR = STAR_WHITE;
constexpr sf::Color MESSAGE_DETAIL_COLOR = PALE_BLUE;

/** @brief The bar under NOT_ON_SERVER_MESSAGE. */
constexpr sf::IntRect JOIN_BAR({160, 160}, BUTTON_SIZE);

/** @brief The chips under the panel, against its right side, and the summary of
 * the filter on their row. */
constexpr int LEADERBOARD_CHIPS_TOP = 224;
constexpr int LEADERBOARD_CHIPS_RIGHT =
    LEADERBOARD_PANEL.position.x + LEADERBOARD_PANEL.size.x;
constexpr sf::Vector2i FILTER_SUMMARY_ANCHOR(LEADERBOARD_PANEL.position.x, 228);
constexpr TextStyle FILTER_SUMMARY_STYLE{
    .font = TextFont::Small, .color = PALE_BLUE, .outlined = true};

}  // namespace rtype::client
