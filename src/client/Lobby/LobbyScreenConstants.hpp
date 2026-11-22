#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BarRowWidth.hpp"
#include "EntitySpriteConstants.hpp"
#include "HangarBayConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TextStyle.hpp"
#include "Translation.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Heading of the waiting room, and the label of its main bar before
 * the player asked to be ready, then once asked. */
constexpr Translation LOBBY_HEADING{.english = "WAITING ROOM",
                                    .french = "SALLE D'ATTENTE"};
constexpr Translation READY_LABEL{.english = "READY", .french = "PRÊT"};
constexpr Translation CANCEL_READY_LABEL{.english = "CANCEL READY",
                                         .french = "ANNULER PRÊT"};

/** @brief The key of the chip of the leaderboard, and what its keycap says. */
constexpr sf::Keyboard::Key LEADERBOARD_KEY = sf::Keyboard::Key::L;
constexpr const char* LEADERBOARD_KEY_NAME = "L";

/** @brief Order of the main bar in the menu of the waiting room, then of the
 * MAP bar beside it. */
constexpr std::size_t READY_BUTTON = 0;
constexpr std::size_t MAP_BUTTON = 1;
constexpr int LOBBY_BAR_COUNT = 2;
constexpr int LOBBY_BARS_WIDTH =
    barRowWidth(LOBBY_BAR_COUNT, BUTTON_SIZE, BUTTON_GAP);

/**
 * @brief The MAP bar: the key that presses it and what its keycap says, the
 * text before the name of the map it shows, and what it says while the
 * server plays one level and lists no map. A name of the longest kind the
 * protocol allows (MAXIMUM_MAP_NAME_SIZE, 16 capitals) does not fit the bar:
 * the maps of a server keep their names short.
 */
constexpr sf::Keyboard::Key MAP_KEY = sf::Keyboard::Key::M;
constexpr const char* MAP_KEY_NAME = "M";
constexpr std::string_view MAP_MARK = "{MAP}";
constexpr Translation MAP_BAR{.english = "MAP: {MAP}",
                              .french = "CARTE : {MAP}"};
constexpr Translation MAP_BAR_CLOSED{.english = "MAP: FIXED",
                                     .french = "CARTE : FIXE"};

/** @brief The vote of a pilot, written in its bay before the name of the
 * map. */
constexpr Translation BAY_VOTE{.english = "VOTE: {MAP}",
                               .french = "VOTE : {MAP}"};

/**
 * @brief The card of the map, painted over the MAP bar: MAP_CARD_RISE above
 * it, as wide, MAP_CARD_HEIGHT tall, so it ends above the chips. Its preview,
 * MAP_PREVIEW_SIZE, lies MAP_PREVIEW_OFFSET in, framed one pixel wide, a
 * MAP_PREVIOUS_MARK MAP_ARROW_GAP to its left and a MAP_NEXT_MARK as far to
 * its right; its words, MAP_CARD_TEXT_LEFT in: MAP_CARD_LABEL, then the name
 * of the map one word per line, MAP_NAME_LINE_STEP apart, MAP_NAME_LINES at
 * most, then a pip of the color of each pilot who votes for it, then the
 * keycap of MAP_KEY in its bottom-right corner, MAP_CARD_INSET in. A preview
 * the pack lacks shows NO_PREVIEW_MARK; a room where nobody voted shows no
 * map, but NO_MAP_CHOSEN_NAME: the vote is drawn at random then.
 */
constexpr int MAP_CARD_RISE = 4;
constexpr int MAP_CARD_HEIGHT = 55;
constexpr sf::Vector2i MAP_PREVIEW_SIZE(80, 45);
constexpr sf::Vector2i MAP_PREVIEW_OFFSET(11, 5);
constexpr int MAP_ARROW_GAP = 6;
constexpr const char* MAP_PREVIOUS_MARK = "<";
constexpr const char* MAP_NEXT_MARK = ">";
constexpr int MAP_CARD_TEXT_LEFT = 101;
constexpr int MAP_CARD_INSET = 3;
/** @brief How wide the words of the card may run: from MAP_CARD_TEXT_LEFT to
 * MAP_CARD_INSET before its right edge. */
constexpr int MAP_NAME_WIDTH =
    BUTTON_SIZE.x - MAP_CARD_TEXT_LEFT - MAP_CARD_INSET;
constexpr Translation MAP_CARD_LABEL{.english = "MAP", .french = "CARTE"};
constexpr int MAP_NAME_TOP = 15;
constexpr int MAP_NAME_LINE_STEP = 10;
constexpr int MAP_NAME_LINES = 3;
constexpr int VOTE_PIP_SIZE = 4;
constexpr int VOTE_PIP_STEP = 6;
constexpr const char* NO_PREVIEW_MARK = "?";
constexpr Translation NO_MAP_CHOSEN_NAME{.english = "RANDOM",
                                         .french = "AU HASARD"};
constexpr const char* MAP_NAME_WORD_SEPARATOR = " ";

/** @brief The number of the first map the room lists: votes count them from
 * 1, network::NO_MAP_VOTE being no vote. */
constexpr std::uint8_t FIRST_MAP_NUMBER = 1;

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
constexpr Translation ALL_READY_STATUS{.english = "ALL READY: LAUNCHING",
                                       .french = "TOUS PRÊTS : DÉCOLLAGE"};
constexpr Translation YOU_NOT_READY_STATUS{.english = "YOU ARE NOT READY",
                                           .french = "VOUS N'ÊTES PAS PRÊT"};
constexpr std::string_view PILOT_MARK = "{PILOT}";
constexpr std::string_view NOT_READY_COUNT_MARK = "{COUNT}";
constexpr Translation PILOT_NOT_READY_STATUS{
    .english = "{PILOT} IS NOT READY", .french = "{PILOT} N'EST PAS PRÊT"};
constexpr Translation PILOTS_NOT_READY_STATUS{
    .english = "{COUNT} PILOTS NOT READY",
    .french = "{COUNT} PILOTES PAS PRÊTS"};

/** @brief The row the lines of the chat end on: the bottom of the HELP and
 * BACK chips, SAFE_MARGIN above the bottom of the picture. */
constexpr int LOBBY_CHAT_BOTTOM = INTERFACE_AREA.size.y - SAFE_MARGIN;

}  // namespace rtype::client
