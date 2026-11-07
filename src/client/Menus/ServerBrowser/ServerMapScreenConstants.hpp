#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>

namespace rtype::client {

/** @brief The header of the map: its title, and the line under it, which
 * starts with the number of servers shown. */
constexpr const char* SERVER_MAP_TITLE = "SECTOR MAP";
constexpr const char* SERVER_COUNT_SUFFIX = " SERVERS";
constexpr const char* ONE_SERVER_TEXT = "1 SERVER";
constexpr const char* SERVER_MAP_HINT =
    " - THE CLOSER TO YOUR PLANET, THE LOWER THE PING";
constexpr sf::Vector2i SERVER_MAP_TITLE_ANCHOR{24, 24};
constexpr sf::Vector2i SERVER_MAP_SUBTITLE_ANCHOR{24, 37};

/** @brief What the map says while it has no server to show, centred on the
 * map, a line every MESSAGE_LINE_PITCH. */
constexpr const char* SEARCHING_TITLE = "SEARCHING";
constexpr const char* SEARCHING_DETAIL =
    "THE LIST COMES FROM THE MASTER SERVER";
constexpr const char* NO_SERVER_TITLE = "NO SERVER ONLINE";
constexpr const char* NO_SERVER_DETAIL = "D: DIRECT CONNECT STILL WORKS";
constexpr const char* MASTER_UNREACHABLE_TITLE = "MASTER UNREACHABLE";
constexpr const char* MASTER_UNREACHABLE_DETAIL =
    "R TO RETRY, M FOR ANOTHER MASTER, D FOR DIRECT CONNECT";
constexpr sf::Vector2i MESSAGE_ANCHOR{240, 120};
constexpr int MESSAGE_LINE_PITCH = 16;

/** @brief How often the list is read again on its own, and the key that reads
 * it at once. */
constexpr std::chrono::seconds SERVER_LIST_REFRESH_INTERVAL{15};
constexpr sf::Keyboard::Key REFRESH_KEY = sf::Keyboard::Key::R;

/** @brief Where a server of another version stands, since it gives no ping:
 * on the farthest ring. */
constexpr double PING_OF_A_SERVER_OF_ANOTHER_VERSION = 225.0;

/** @brief The minimap in the bottom right corner of the map, while zoomed
 * in. */
constexpr sf::IntRect MINIMAP_AREA{{368, 178}, {72, 30}};

/** @brief Where the names of the stars may not go besides the bubble of the
 * chosen server: the minimap, as the mockup keeps it free. */
constexpr sf::IntRect MINIMAP_KEEP_OUT{{366, 176}, {76, 34}};

/** @brief The bubble of the chosen server, BUBBLE_WIDTH wide, BUBBLE_OFFSET
 * from the star, on its left when the star lies right of BUBBLE_FLIP_X. */
constexpr int BUBBLE_WIDTH = 120;
constexpr sf::Vector2i BUBBLE_OFFSET{14, -8};
constexpr int BUBBLE_FLIP_X = 240;
constexpr const char* PILOTS_AND_GAMES_JOIN = " PILOTS - ";
constexpr const char* GAMES_WORD = " GAMES";
constexpr const char* PING_AND_VERSION_JOIN = " MS - ";
constexpr const char* NO_PING_TEXT = "--";
constexpr const char* SLOW_WARNING = "SLOW: THE SERVER STRUGGLES";
constexpr const char* UPDATE_WARNING = "UPDATE: ANOTHER VERSION";
constexpr const char* OPEN_GAMES_HINT = "ENTER: ITS GAMES";
constexpr const char* ZOOM_IN_HINT = "ENTER OR SCROLL: ZOOM IN";

/** @brief The chips under the map: QUICK PLAY on the left, then RANKING,
 * ADDRESS and BACK against the right edge of the map. */
constexpr int SERVER_MAP_CHIPS_TOP = 226;
constexpr int SERVER_MAP_CHIPS_LEFT = 24;
constexpr int SERVER_MAP_CHIPS_RIGHT = 456;
constexpr const char* QUICK_PLAY_LABEL = "QUICK PLAY";
constexpr const char* QUICK_PLAY_KEY_NAME = "Q";
constexpr sf::Keyboard::Key QUICK_PLAY_KEY = sf::Keyboard::Key::Q;
constexpr const char* RANKING_LABEL = "RANKING";
constexpr const char* RANKING_KEY_NAME = "L";
constexpr sf::Keyboard::Key RANKING_KEY = sf::Keyboard::Key::L;
constexpr const char* ADDRESS_LABEL = "ADDRESS";
constexpr const char* ADDRESS_KEY_NAME = "D";
constexpr sf::Keyboard::Key ADDRESS_KEY = sf::Keyboard::Key::D;

/** @brief The hangar star: where it stands on the whole map, close to the
 * planet, where no server stands (servers start past the reach of the planet
 * and its ring, 120 pixels from its centre); its size; and its bubble,
 * whose hint names HANGAR_KEY_NAME between its start and its end. */
constexpr sf::Vector2i HANGAR_STAR_POINT{84, 64};
constexpr int HANGAR_STAR_RADIUS = 3;
constexpr const char* HANGAR_STAR_DETAIL = "LOCKER AND SHIP";
constexpr const char* HANGAR_STAR_HINT_START = "CLICK OR ";
constexpr const char* HANGAR_STAR_HINT_END = ": OPEN";

/** @brief The chip that opens the form of the master, on the right of the
 * title, and where the master read is written under it, ending at its right
 * edge. */
constexpr const char* MASTER_LABEL = "MASTER";
constexpr const char* MASTER_KEY_NAME = "M";
constexpr sf::Keyboard::Key MASTER_KEY = sf::Keyboard::Key::M;
constexpr int MASTER_CHIP_TOP = 20;
constexpr sf::Vector2i MASTER_ADDRESS_ANCHOR{SERVER_MAP_CHIPS_RIGHT, 40};

/** @brief The panel of the games of a server, over the map dimmed: its area,
 * its title, the count of its games, its rule, its columns, its rows, and the
 * line under them that says what ENTER does. */
constexpr sf::IntRect GAME_PANEL_AREA{{184, 52}, {272, 168}};
constexpr int GAME_PANEL_PADDING = 10;
constexpr int GAME_PANEL_TITLE_TOP = 6;
constexpr int GAME_PANEL_RULE_TOP = 18;
constexpr int GAME_PANEL_COLUMNS_TOP = 24;
constexpr int GAME_PANEL_FIRST_ROW = 37;
constexpr int GAME_ROW_PITCH = 15;
constexpr std::size_t GAME_ROWS_SHOWN = 6;
constexpr int GAME_NAME_COLUMN = 12;
constexpr int GAME_PILOTS_COLUMN = 120;
constexpr int GAME_STATE_COLUMN = 176;
constexpr int GAME_ROW_HIGHLIGHT_LEFT = 8;
constexpr int GAME_ROW_HIGHLIGHT_RISE = 2;
constexpr int GAME_ROW_HIGHLIGHT_HEIGHT = 12;
constexpr int GAME_PANEL_DESCRIPTION_TOP = 152;
constexpr const char* GAME_COLUMN_LABEL = "GAME";
constexpr const char* PILOTS_COLUMN_LABEL = "PILOTS";
constexpr const char* STATE_COLUMN_LABEL = "STATE";
constexpr const char* PILOTS_SEPARATOR = "/";
constexpr const char* WAITING_WORD = "WAITING";
constexpr const char* IN_GAME_WORD = "IN GAME";
constexpr const char* FULL_WORD = "FULL";
constexpr const char* JOINABLE_DESCRIPTION = ": ENTER TO JOIN.";
constexpr const char* IN_GAME_DESCRIPTION = " IS IN GAME: IT CANNOT BE JOINED.";
constexpr const char* FULL_DESCRIPTION = " IS FULL: IT CANNOT BE JOINED.";
constexpr const char* NO_GAME_TITLE = "NO GAME YET";
constexpr const char* SEARCHING_GAMES_TITLE = "ASKING THE MASTER...";

}  // namespace rtype::client
