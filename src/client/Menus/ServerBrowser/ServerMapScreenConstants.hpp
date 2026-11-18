#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <optional>
#include "CasinoScreenIdConstants.hpp"
#include "HangarScreenIdConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KopecksLabelConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "SectorMapConstants.hpp"
#include "ServerMapTab.hpp"
#include "ShopScreenIdConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The title of the map, under the tabs, SERVER_MAP_TITLE_GAP over
 * the top left corner of the map. */
constexpr const char* SERVER_MAP_TITLE = "SECTOR MAP";
constexpr int SERVER_MAP_TITLE_GAP = 4;
constexpr sf::Vector2i SERVER_MAP_TITLE_ANCHOR{
    SECTOR_MAP_AREA.position.x,
    SECTOR_MAP_AREA.position.y - TEXT_LINE_HEIGHT - SERVER_MAP_TITLE_GAP};

/**
 * @brief The tabs over the map, the Tab Rail v2 of the design system: SECTORS,
 * the map itself, open; HANGAR, opened by HANGAR_KEY; SHOP, opened by
 * SHOP_KEY; CASINO, opened by CASINO_KEY. Signed out, the shop and the casino
 * open the dialog that asks to sign in. The row is centred between the safe
 * margin and the kopecks balance, BUTTON_GAP short of its frame, its labels at
 * SERVER_MAP_TABS_TOP, halfway into the safe margin, so the bar under them
 * stands well clear of the title. The kopecks frame rises with them: its
 * amount, KOPECKS_FRAME_PADDING under its top, sits on the row of the labels.
 */
constexpr const char* SECTORS_TAB_LABEL = "SECTORS";
constexpr const char* CASINO_TAB_LABEL = "CASINO";
constexpr sf::Keyboard::Key CASINO_KEY = sf::Keyboard::Key::C;
constexpr std::size_t SECTORS_TAB = 0;
constexpr std::array<ServerMapTab, 4> SERVER_MAP_TABS{
    ServerMapTab{.tab = {.label = SECTORS_TAB_LABEL, .locked = false},
                 .key = std::nullopt,
                 .screen = std::nullopt},
    ServerMapTab{.tab = {.label = HANGAR_LABEL, .locked = false},
                 .key = HANGAR_KEY,
                 .screen = HANGAR_SCREEN},
    ServerMapTab{.tab = {.label = SHOP_LABEL, .locked = false},
                 .key = SHOP_KEY,
                 .screen = SHOP_SCREEN},
    ServerMapTab{.tab = {.label = CASINO_TAB_LABEL, .locked = false},
                 .key = CASINO_KEY,
                 .screen = CASINO_SCREEN}};
constexpr int SERVER_MAP_TABS_TOP = SAFE_MARGIN / 2;
constexpr sf::Vector2i SERVER_MAP_TABS_TOP_MIDDLE{
    (SAFE_MARGIN + KOPECKS_BALANCE_TOP_RIGHT.x - KOPECKS_FRAME_SIZE.x -
     BUTTON_GAP) /
        2,
    SERVER_MAP_TABS_TOP};
constexpr sf::Vector2i SERVER_MAP_KOPECKS_TOP_RIGHT{
    KOPECKS_BALANCE_TOP_RIGHT.x, SERVER_MAP_TABS_TOP - KOPECKS_FRAME_PADDING.y};

/** @brief What the map says while it has no server to show, centred on the
 * map, a line every MESSAGE_LINE_PITCH. */
constexpr const char* SEARCHING_TITLE = "SEARCHING";
constexpr const char* SEARCHING_DETAIL =
    "THE LIST COMES FROM THE MASTER SERVER";
constexpr const char* NO_SERVER_TITLE = "NO SERVER ONLINE";
constexpr const char* NO_SERVER_DETAIL = "D: DIRECT CONNECT STILL WORKS";
constexpr const char* MASTER_UNREACHABLE_TITLE = "MASTER UNREACHABLE";
constexpr const char* MASTER_UNREACHABLE_DETAIL =
    "R TO RETRY, D FOR DIRECT CONNECT";
constexpr sf::Vector2i MESSAGE_ANCHOR{240, 120};
constexpr int MESSAGE_LINE_PITCH = 16;

/** @brief How often the list is read again on its own, and the key that reads
 * it at once. */
constexpr std::chrono::seconds SERVER_LIST_REFRESH_INTERVAL{15};
constexpr sf::Keyboard::Key REFRESH_KEY = sf::Keyboard::Key::R;

/** @brief How long after the player came back from a server the list is read
 * again: twice network::MINIMUM_NOTICE_GAP, the least time between two
 * heartbeats that tell the master the games changed, so a player who left is
 * no longer counted, unless the master or the server is slow. */
constexpr std::chrono::milliseconds GAMES_RECHECK_DELAY =
    2 * network::MINIMUM_NOTICE_GAP;

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

/** @brief The chips under the map: QUICK PLAY on the left, then ADDRESS
 * and BACK against the right edge of the map. */
constexpr int SERVER_MAP_CHIPS_GAP = 6;
constexpr int SERVER_MAP_CHIPS_TOP =
    SECTOR_MAP_AREA.position.y + SECTOR_MAP_AREA.size.y + SERVER_MAP_CHIPS_GAP;
constexpr int SERVER_MAP_CHIPS_LEFT = SECTOR_MAP_AREA.position.x;
constexpr int SERVER_MAP_CHIPS_RIGHT =
    SECTOR_MAP_AREA.position.x + SECTOR_MAP_AREA.size.x;
constexpr const char* QUICK_PLAY_LABEL = "QUICK PLAY";
constexpr const char* QUICK_PLAY_KEY_NAME = "Q";
constexpr sf::Keyboard::Key QUICK_PLAY_KEY = sf::Keyboard::Key::Q;
constexpr const char* ADDRESS_LABEL = "ADDRESS";
constexpr const char* ADDRESS_KEY_NAME = "D";
constexpr sf::Keyboard::Key ADDRESS_KEY = sf::Keyboard::Key::D;

/** @brief The panel of the games of a server, over the map dimmed: its area,
 * as tall as the map, its title, the count of its games, its rule, its
 * columns, its rows, and the line under them that says what ENTER does. */
constexpr int GAME_PANEL_LEFT = 184;
constexpr sf::IntRect GAME_PANEL_AREA{
    {GAME_PANEL_LEFT, SECTOR_MAP_AREA.position.y},
    {SECTOR_MAP_AREA.position.x + SECTOR_MAP_AREA.size.x - GAME_PANEL_LEFT,
     SECTOR_MAP_AREA.size.y}};
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

/** @brief The row that opens a new game, first in the panel once the master
 * answered: its label, the line that says what ENTER does, and the row of the
 * message under it when the server has no game. */
constexpr const char* NEW_GAME_ROW_LABEL = "+ NEW GAME";
constexpr const char* NEW_GAME_DESCRIPTION =
    "+ NEW GAME: ENTER TO SET ITS RULES.";
constexpr int GAME_PANEL_EMPTY_MESSAGE_ROW =
    GAME_PANEL_FIRST_ROW + (2 * GAME_ROW_PITCH);
constexpr const char* SEARCHING_GAMES_TITLE = "ASKING THE MASTER...";

}  // namespace rtype::client
