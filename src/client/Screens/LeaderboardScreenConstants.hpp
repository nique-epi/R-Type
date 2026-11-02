#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "FireColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "ItemListColumn.hpp"
#include "ItemListLayout.hpp"
#include "LeaderboardFigure.hpp"
#include "LeaderboardMessage.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The tabs of the screen. */
constexpr const char* SERVER_TAB_LABEL = "SERVER";
constexpr const char* GLOBAL_TAB_LABEL = "GLOBAL";

/** @brief The chips of the screen and the bar under its message. */
constexpr const char* FILTER_LABEL = "FILTER";
constexpr sf::Keyboard::Key FILTER_KEY = sf::Keyboard::Key::F;
constexpr const char* FILTER_KEY_NAME = "F";
constexpr const char* JOIN_LABEL = "JOIN A SERVER";

/** @brief Texts of the filter dialog. */
constexpr const char* DIFFICULTY_ROW_LABEL = "DIFFICULTY";
constexpr const char* PILOTS_ROW_LABEL = "PILOTS";
constexpr const char* APPLY_LABEL = "APPLY";

/** @brief The filters a player can pick: the names of the waves file, in the
 * order of their labels; no value ALL, since two difficulties never mix. */
constexpr std::array<std::string_view, 3> LEADERBOARD_DIFFICULTY_NAMES{
    "easy", "normal", "hard"};
constexpr std::array<const char*, 3> LEADERBOARD_DIFFICULTY_LABELS{
    "EASY", "NORMAL", "HARD"};
constexpr std::array<const char*, 4> LEADERBOARD_PILOT_LABELS{"1", "2", "3",
                                                              "4"};
static_assert(LEADERBOARD_PILOT_LABELS.size() == network::MAXIMUM_PLAYERS,
              "the filter offers every number of players of a game");

/** @brief The filter a screen opens on, without a waiting room to read a number
 * of pilots from. */
constexpr std::size_t DEFAULT_DIFFICULTY_CHOICE = 1;

/** @brief The summary under the panel, between the difficulty and the number of
 * pilots. */
constexpr const char* SUMMARY_GAMES_TEXT = " GAMES, ";

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
constexpr sf::IntRect LEADERBOARD_RULE({80, 40}, {320, 1});
constexpr sf::Vector2i LEADERBOARD_TABS_TOP_LEFT(80, 50);
constexpr int LEADERBOARD_TABS_WIDTH = 320;
constexpr sf::IntRect LEADERBOARD_PANEL({80, 66}, {320, 150});

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
 * than the 10 of the first versions of the design. */
constexpr ItemListLayout LEADERBOARD_LIST{.topLeft = {92, 73},
                                          .width = 296,
                                          .rowPitch = 13,
                                          .rowCapacity = 10,
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
constexpr std::array<const char*, 12> MONTH_NAMES{"JAN", "FEB", "MAR", "APR",
                                                  "MAY", "JUN", "JUL", "AUG",
                                                  "SEP", "OCT", "NOV", "DEC"};
constexpr std::int64_t LAST_SHOWN_DATE_SECONDS = 4102444799;
constexpr const char* UNKNOWN_DATE_TEXT = "-- --- --";
constexpr unsigned int FIRST_DAY_OF_TWO_DIGITS = 10;
constexpr int YEARS_PER_CENTURY = 100;

/** @brief What the panel says in place of rows. */
constexpr LeaderboardMessage EMPTY_LEADERBOARD_MESSAGE{
    .line = "NO SCORE IN THIS MODE YET", .detail = "BE THE FIRST TO FLY IT"};
constexpr LeaderboardMessage NOT_ON_SERVER_MESSAGE{
    .line = "NOT ON A SERVER", .detail = "JOIN ONE TO READ ITS RANKING"};
constexpr LeaderboardMessage NO_ANSWER_MESSAGE{
    .line = "NO ANSWER FROM THE SERVER",
    .detail = "APPLY A FILTER TO ASK AGAIN"};
constexpr LeaderboardMessage OUT_OF_REACH_MESSAGE{
    .line = "THE MASTER IS OUT OF REACH",
    .detail = "THE SERVER TAB STILL WORKS"};
constexpr const char* ASKING_TEXT = "ASKING THE SERVER";
constexpr const char* NO_OWN_SCORE_TEXT = "YOU HAVE NO SCORE IN THIS MODE YET";

/** @brief A dot more after ASKING_TEXT each ASKING_DOT_TIME, up to three, then
 * none again. */
constexpr engine::Duration ASKING_DOT_TIME = std::chrono::milliseconds(300);
constexpr int ASKING_DOT_STEPS = 4;

/** @brief Where the panel writes them: its line and its small print, centered;
 * ASKING_TEXT from a fixed left, so its dots never move it. */
constexpr sf::Vector2i MESSAGE_LINE_ANCHOR(240, 130);
constexpr sf::Vector2i MESSAGE_DETAIL_ANCHOR(240, 144);
constexpr sf::Vector2i ASKING_ANCHOR(160, 138);
constexpr sf::Color MESSAGE_LINE_COLOR = STAR_WHITE;
constexpr sf::Color MESSAGE_DETAIL_COLOR = PALE_BLUE;

/** @brief The bar under NOT_ON_SERVER_MESSAGE. */
constexpr sf::IntRect JOIN_BAR({160, 160}, BUTTON_SIZE);

/** @brief The chips under the panel, against its right side, and the summary of
 * the filter on their row. */
constexpr int LEADERBOARD_CHIPS_TOP = 224;
constexpr int LEADERBOARD_CHIPS_RIGHT = 400;
constexpr sf::Vector2i FILTER_SUMMARY_ANCHOR(80, 228);
constexpr TextStyle FILTER_SUMMARY_STYLE{
    .font = TextFont::Small, .color = PALE_BLUE, .outlined = true};

}  // namespace rtype::client
