#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TextMetrics.hpp"

namespace rtype::client::testing {

/** @brief The servers of the map of the tests: Lyon nearer than Paris, both of
 * this version, and an old station of another version, which gives no ping. */
constexpr std::int64_t LYON_SERVER_ID = 8;
constexpr std::int64_t PARIS_SERVER_ID = 7;
constexpr std::int64_t OLD_STATION_ID = 9;
constexpr std::string_view LYON_SERVER_NAME = "LYON-1";
constexpr std::string_view PARIS_SERVER_NAME = "PARIS-1";
constexpr std::string_view OLD_STATION_NAME = "OLD-STATION";
constexpr std::string_view LYON_SERVER_ADDRESS = "203.0.113.8";
constexpr std::string_view PARIS_SERVER_ADDRESS = "203.0.113.7";
constexpr std::string_view OLD_STATION_ADDRESS = "203.0.113.9";
constexpr std::uint16_t SERVER_GAME_PORT = 4242;
constexpr std::chrono::milliseconds LYON_PING{24};
constexpr std::chrono::milliseconds PARIS_PING{88};
/** @brief Where Paris comes in the list of three servers, sorted by ping. */
constexpr std::size_t PARIS_INDEX = 1;
constexpr std::uint8_t PLAYERS_PER_GAME = 4;
/** @brief The players a server of the tests holds at most, four games of
 * four, and one fewer: its last seat still free. */
constexpr std::uint16_t SERVER_SEATS = 16;
constexpr std::uint16_t ONE_SEAT_LEFT = 15;

/** @brief The games of Lyon: one waiting, one in game. */
constexpr std::uint16_t WAITING_GAME_NUMBER = 1;
constexpr std::uint16_t PLAYING_GAME_NUMBER = 2;
constexpr std::string_view WAITING_GAME_NAME = "ALPHA WING";

/**
 * @brief The largest text size; the pilots of the waiting game of Lyon of its
 * seats, written on the second line of its row at that size; enough Page Down
 * to reach the bottom of the map; and the middle of BACK once the map is
 * scrolled to its bottom: the chips are 24 rows tall on rows 260 and 292 under
 * a map moved 30 rows down, the picture 336 tall scrolls 66 rows, so BACK
 * spans rows 226 to 250, from 330 to 456.
 */
constexpr TextMetrics LARGEST_MAP_TEXT{.halfPixelsPerFontPixel = 4};
constexpr std::string_view WAITING_GAME_SEATS = "2/4";
constexpr int PAGES_TO_THE_BOTTOM = 3;
constexpr sf::Vector2i SCROLLED_BACK_MIDDLE(390, 238);
constexpr std::string_view PLAYING_GAME_NAME = "LAST STAND";
constexpr std::uint8_t TWO_PILOTS = 2;

/** @brief A frame, the time after which the map reads the list again on its
 * own (15 s), and a time short of it. */
constexpr std::chrono::milliseconds ONE_FRAME{16};

/** @brief Where the hangar star stands on the whole map, close to the planet,
 * and the line of its bubble that tells how to open it. */
constexpr sf::Vector2i HANGAR_STAR_ON_THE_MAP{84, 64};
constexpr std::string_view HANGAR_STAR_HINT_LINE = "CLICK OR G: OPEN";

/** @brief The house the hangar star is drawn as, 9 pixels at top 47 of the
 * sheet of the map, and where it lands: centered on (84, 64), from (80, 60). */
constexpr std::string_view MAP_SHEET = "sprites/sector_map.png";
constexpr sf::IntRect HANGAR_HOUSE_SOURCE{{0, 47}, {9, 9}};
constexpr sf::Vector2i HANGAR_HOUSE_ON_THE_MAP{80, 60};

/** @brief Where the shop stands on the whole map, left of the hangar, the
 * lines of its bubble, and the kiosk it is drawn as: 9 pixels at top 58 of
 * the sheet, centered on (36, 62), so from (32, 58). */
constexpr sf::Vector2i SHOP_ON_THE_MAP{36, 62};
constexpr std::string_view SHOP_HINT_LINE = "CLICK OR S: OPEN";
constexpr std::string_view SHOP_DETAIL_LINE = "HATS AND SHIPS";
constexpr sf::IntRect SHOP_KIOSK_SOURCE{{0, 58}, {9, 9}};
constexpr sf::Vector2i SHOP_KIOSK_ON_THE_MAP{32, 58};

/** @brief One frame of the turn of a planet. */
constexpr std::chrono::milliseconds ONE_PLANET_TURN_FRAME{150};
constexpr std::chrono::seconds REFRESH_INTERVAL{15};
constexpr std::chrono::seconds SHORT_OF_THE_REFRESH{14};

/** @brief The words the map writes when the master is unreachable. */
constexpr std::string_view UNREACHABLE_MASTER = "MASTER UNREACHABLE";

/** @brief Seven games, one more than the panel shows at once. */
constexpr std::size_t SEVEN_GAMES = 7;
constexpr std::uint16_t SEVENTH_GAME_NUMBER = 7;
/** @brief The rows down from + NEW GAME to the seventh game, and to the game
 * in progress of Lyon, under its waiting game. */
constexpr int SEVEN_ROWS_DOWN = 7;
constexpr int PLAYING_GAME_ROW = 2;

/** @brief Pixels of the panel of the games of Lyon, which lies from (184, 52),
 * its first row 37 pixels under its top and its rows 15 pixels apart: on
 * + NEW GAME, on the waiting game, on the game in progress, and on the
 * headings of its columns, 24 pixels under its top, above every row. */
constexpr sf::Vector2i NEW_GAME_ROW_ON_THE_PANEL{220, 89};
constexpr sf::Vector2i WAITING_GAME_ROW_ON_THE_PANEL{220, 104};
constexpr sf::Vector2i PLAYING_GAME_ROW_ON_THE_PANEL{220, 119};
constexpr sf::Vector2i COLUMN_HEADINGS_ON_THE_PANEL{220, 76};

/** @brief The lines under the rows a player must read: on + NEW GAME, and on
 * the waiting game of Lyon, played by the default rules. */
constexpr std::string_view NEW_GAME_LINE =
    "+ NEW GAME: ENTER TO SET ITS RULES.";
constexpr std::string_view WAITING_GAME_LINE =
    "ALPHA WING, NORMAL, 3 LIVES, FRIENDLY FIRE OFF: ENTER TO JOIN.";

}  // namespace rtype::client::testing
