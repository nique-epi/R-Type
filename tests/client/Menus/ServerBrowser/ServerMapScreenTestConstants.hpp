#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

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
constexpr std::string_view PLAYING_GAME_NAME = "LAST STAND";
constexpr std::uint8_t TWO_PILOTS = 2;

/** @brief A frame, the time after which the map reads the list again on its
 * own (15 s), and a time short of it. */
constexpr std::chrono::milliseconds ONE_FRAME{16};

/** @brief The sheet of the map, and two of its glyphs the map no longer
 * draws: the house of the hangar, 9 pixels at top 47, and the kiosk of the
 * shop, 9 pixels at top 58. */
constexpr std::string_view MAP_SHEET = "sprites/sector_map.png";
constexpr sf::IntRect HANGAR_HOUSE_SOURCE{{0, 47}, {9, 9}};
constexpr sf::IntRect SHOP_KIOSK_SOURCE{{0, 58}, {9, 9}};

/**
 * @brief The tabs over the map, as the code lays them out: centred on x 194,
 * halfway between the safe margin (24) and the kopecks frame (84 wide, ending
 * at 456) less a gap of 8; SECTORS from x 56 to 112, HANGAR from 132 to 180,
 * SHOP from 200 to 232, CASINO and its badge from 252 to 332; the labels at
 * row 12, half the safe margin, a tab answers from row 8 to row 25. A point
 * inside HANGAR, one in the gap between SECTORS and HANGAR, one on CASINO.
 */
constexpr sf::Vector2i IN_THE_HANGAR_TAB{150, 16};
constexpr sf::Vector2i BETWEEN_SECTORS_AND_HANGAR{120, 16};
constexpr sf::Vector2i ON_THE_CASINO_TAB{270, 16};

/** @brief The kopecks of the player of the tests, and how the balance writes
 * them. */
constexpr std::uint32_t PLAYER_KOPECKS = 20;
constexpr std::string_view PLAYER_KOPECKS_TEXT = "20";

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

/** @brief Pixels of the panel of the games of Lyon, which lies from (184, 56),
 * its first row 37 pixels under its top and its rows 15 pixels apart: on
 * + NEW GAME, on the waiting game, on the game in progress, and on the
 * headings of its columns, 24 pixels under its top, above every row. */
constexpr sf::Vector2i NEW_GAME_ROW_ON_THE_PANEL{220, 93};
constexpr sf::Vector2i WAITING_GAME_ROW_ON_THE_PANEL{220, 108};
constexpr sf::Vector2i PLAYING_GAME_ROW_ON_THE_PANEL{220, 123};
constexpr sf::Vector2i COLUMN_HEADINGS_ON_THE_PANEL{220, 80};

/** @brief The lines under the rows a player must read: on + NEW GAME, and on
 * the waiting game of Lyon, played by the default rules. */
constexpr std::string_view NEW_GAME_LINE =
    "+ NEW GAME: ENTER TO SET ITS RULES.";
constexpr std::string_view WAITING_GAME_LINE =
    "ALPHA WING, NORMAL, 3 LIVES, FRIENDLY FIRE OFF: ENTER TO JOIN.";
constexpr std::string_view FRENCH_WAITING_GAME_LINE =
    "ALPHA WING, NORMAL, 3 VIES, TIR AMI COUPÉ : ENTRÉE POUR REJOINDRE.";

}  // namespace rtype::client::testing
