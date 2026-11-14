#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include "BarRowWidth.hpp"
#include "EntitySpriteConstants.hpp"
#include "HangarBayConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Heading of the waiting room, and the label of its main bar before
 * the player asked to be ready, then once asked. */
constexpr const char* LOBBY_HEADING = "WAITING ROOM";
constexpr const char* READY_LABEL = "READY";
constexpr const char* CANCEL_READY_LABEL = "CANCEL READY";

/** @brief The key of the chip of the leaderboard, and what its keycap says. */
constexpr sf::Keyboard::Key LEADERBOARD_KEY = sf::Keyboard::Key::L;
constexpr const char* LEADERBOARD_KEY_NAME = "L";

/** @brief Order of the main bar and of BACK in the menu of the waiting
 * room. */
constexpr std::size_t READY_BUTTON = 0;
constexpr std::size_t ROOM_BACK_BUTTON = 1;

/**
 * @brief How far a notch of the wheel scrolls the room, and the keys of the
 * menu that move its selection and scroll the room to it.
 */
constexpr int ROOM_SCROLL_STEP = 16;
constexpr std::array ROOM_SELECTION_KEYS{
    sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::Tab};

/** @brief One bay per player number, side by side under the rule, as wide as
 * it. */
constexpr int LOBBY_BAY_COUNT = static_cast<int>(network::MAXIMUM_PLAYERS);
constexpr sf::Vector2i LOBBY_BAYS_SIZE(barRowWidth(LOBBY_BAY_COUNT,
                                                   HANGAR_BAY_SIZE,
                                                   HANGAR_BAY_GAP),
                                       HANGAR_BAY_SIZE.y);
static_assert(LOBBY_BAYS_SIZE.x == ROOM_RULE_SIZE.x,
              "the bays fill the width of the rule");
static_assert(PLAYER_COLORS.size() ==
                  static_cast<std::size_t>(PLAYER_COLOR_COUNT),
              "every ship color of the entity sheet has its interface color");
static_assert(LOBBY_BAY_COUNT <= PLAYER_COLOR_COUNT,
              "the entity sheet holds a ship color for every player");

/**
 * @brief The number of pilots listed: its end against the right side of the
 * safe margin, on the row of the heading, as a line of a page; then the word
 * after the number, for one pilot and for several.
 */
constexpr sf::Vector2i PILOT_COUNT_ANCHOR(INTERFACE_AREA.size.x - SAFE_MARGIN,
                                          SAFE_MARGIN);
constexpr TextStyle PILOT_COUNT_STYLE{.font = TextFont::Regular,
                                      .color = PAGE_TEXT_COLOR,
                                      .outlined = true,
                                      .alignment = TextAlignment::Right};

/**
 * @brief The rules of the game, in small print on the row of the heading,
 * centered between the end of the heading and the start of the count of
 * pilots, their baseline on the baseline of both. Measured in Silkscreen at
 * 8 px, the longest line, a name of 16 capitals, 9 LIVES and FRIENDLY FIRE OFF,
 * is 251 px wide; between WAITING ROOM (ends at x 120) and 4 PILOTS (starts at
 * x 392) lie 272.
 */
constexpr int GAME_RULES_TOP =
    SAFE_MARGIN + REGULAR_FONT_BASELINE_ROW - SMALL_FONT_BASELINE_ROW;
constexpr TextStyle GAME_RULES_STYLE{.font = TextFont::Small,
                                     .color = PAGE_TEXT_COLOR,
                                     .outlined = true,
                                     .alignment = TextAlignment::Center};

/**
 * @brief The status line under the main bar: every listed pilot ready; the
 * local player the only one not ready; another pilot the only one, after its
 * nickname; several pilots not ready, after their number. It never counts
 * places still open: a game starts with whoever is listed.
 */
constexpr const char* ALL_READY_STATUS = "ALL READY: LAUNCHING";
constexpr const char* YOU_NOT_READY_STATUS = "YOU ARE NOT READY";
constexpr const char* PILOT_NOT_READY_STATUS = " IS NOT READY";
constexpr const char* PILOTS_NOT_READY_STATUS = " PILOTS NOT READY";

/** @brief The row the lines of the chat end on: the bottom of the HELP and
 * BACK chips, SAFE_MARGIN above the bottom of the picture. */
constexpr int LOBBY_CHAT_BOTTOM = INTERFACE_AREA.size.y - SAFE_MARGIN;

}  // namespace rtype::client
