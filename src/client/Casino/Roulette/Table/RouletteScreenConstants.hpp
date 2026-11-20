#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include "CasinoBar.hpp"
#include "CasinoSpriteConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief Where the table and the wheel lie over the room: the table across
 * the screen, the cloth on it, the wheel on its left. */
constexpr sf::Vector2i ROULETTE_TABLE_TOP_LEFT(0, 78);
constexpr int ROULETTE_TABLE_HEIGHT = 150;
constexpr sf::Vector2i WHEEL_TOP_LEFT(18, 89);

/**
 * @brief The butler, croupier behind the table: the top CROUPIER_HEIGHT rows
 * of a frame of his sheet, the table hiding the rest. He stands idle, round
 * and round; while the bets are open, he greets the players once every
 * CROUPIER_INVITE_CYCLE, his greeting starting CROUPIER_INVITE_AFTER into
 * it. With less motion, he holds a frame of his greeting while the bets are
 * open, his first idle frame otherwise.
 */
constexpr engine::Duration CROUPIER_INVITE_CYCLE =
    std::chrono::milliseconds(2600);
constexpr engine::Duration CROUPIER_INVITE_AFTER =
    std::chrono::milliseconds(2000);
constexpr sf::Vector2i CROUPIER_TOP_LEFT(222, 6);
constexpr int CROUPIER_HEIGHT = 76;
constexpr int CROUPIER_HELD_GREETING_FRAME = 2;

/**
 * @brief The plaque under the croupier while the bets are open: SPIN IN and
 * the seconds begun before the wheel spins, over a bar that drains, full
 * SPIN_COUNTDOWN_SPAN before the spin, the default betting time of the
 * master, and full still while more is left; the text and the bar turn red
 * once SPIN_HURRY_SECONDS or fewer are left.
 */
constexpr sf::IntRect SPIN_PLAQUE_AREA({210, 62}, {104, 16});
constexpr sf::Vector2i SPIN_PLAQUE_TEXT_TOP_LEFT(216, 64);
constexpr sf::IntRect SPIN_PLAQUE_BAR({214, 73}, {96, 2});
constexpr const char* SPIN_IN_START = "SPIN IN ";
constexpr engine::Duration SPIN_COUNTDOWN_SPAN = std::chrono::seconds(20);
constexpr std::int64_t SPIN_HURRY_SECONDS = 3;

/** @brief The keys that move the cursor over the cloth, and the one that lays
 * a chip of the value chosen where it stands. */
constexpr sf::Keyboard::Key CURSOR_LEFT_KEY = sf::Keyboard::Key::Left;
constexpr sf::Keyboard::Key CURSOR_RIGHT_KEY = sf::Keyboard::Key::Right;
constexpr sf::Keyboard::Key CURSOR_UP_KEY = sf::Keyboard::Key::Up;
constexpr sf::Keyboard::Key CURSOR_DOWN_KEY = sf::Keyboard::Key::Down;
constexpr sf::Keyboard::Key LAY_CHIP_KEY = MENU_CONFIRM_KEY;

/** @brief The chip the table opens on: the third, of 25. */
constexpr std::size_t FIRST_CHOSEN_CHIP = 2;

/**
 * @brief A stack of chips on the cloth: a chip per value, the largest at the
 * bottom, each CLOTH_CHIP_LIFT pixels above the one under it, at most
 * CLOTH_STACK_CHIPS of them; the corners of the cursor, and those of the
 * number drawn, CLOTH_CORNERS_MARGIN out of their box.
 */
constexpr int CLOTH_CHIP_LIFT = 2;

/**
 * @brief The stacks of the other players seated in the room: their chips
 * multiplied by the color of each player, at OTHER_STACK_ALPHA of 255, and
 * moved aside, each other player in turn by the next of OTHER_STACK_OFFSETS,
 * so the stacks of the player, drawn over them, stay readable.
 */
constexpr std::uint8_t OTHER_STACK_ALPHA = 140;
constexpr std::array OTHER_STACK_OFFSETS{
    sf::Vector2i(-5, 2), sf::Vector2i(5, 2), sf::Vector2i(-5, -2),
    sf::Vector2i(5, -2)};
constexpr std::size_t CLOTH_STACK_CHIPS = 5;
constexpr int CLOTH_CORNERS_MARGIN = 1;

/** @brief The band at the foot of the screen, BAND_AREA: two rules of
 * gold, the chips and the stake, each under its caption, the status line,
 * then the chips of the selector from BAND_FIRST_CHIP. */
constexpr sf::IntRect BAND_AREA({0, 226}, {480, 44});
constexpr sf::Vector2i BAND_CHIPS_CAPTION(10, 235);
constexpr sf::Vector2i BAND_CHIPS_VALUE(10, 245);
constexpr sf::Vector2i BAND_STAKE_CAPTION(70, 235);
constexpr sf::Vector2i BAND_STAKE_VALUE(70, 245);
constexpr sf::Vector2i BAND_STATUS(10, 258);
constexpr sf::Vector2i BAND_FIRST_CHIP(150, 235);
constexpr const char* BAND_CHIPS_CAPTION_TEXT = "CHIPS";
constexpr const char* BAND_STAKE_CAPTION_TEXT = "STAKE";

/** @brief The bars of the band: CLEAR and REBET side by side, BACK under
 * REBET. */
constexpr CasinoBar CLEAR_BAR{.label = "CLEAR",
                              .keyName = "C",
                              .area = sf::IntRect({312, 234}, {78, 16}),
                              .key = sf::Keyboard::Key::C};
constexpr CasinoBar REBET_BAR{.label = "REBET",
                              .keyName = "R",
                              .area = sf::IntRect({396, 234}, {78, 16}),
                              .key = sf::Keyboard::Key::R};
constexpr CasinoBar TABLE_BACK_BAR{.label = BACK_LABEL,
                                   .keyName = CANCEL_KEY_NAME,
                                   .area = sf::IntRect({396, 253}, {78, 14}),
                                   .key = MENU_CANCEL_KEY};

/**
 * @brief What the status line says: the table closed before the first state
 * of the room, no chip held, the bets open with the seconds before they
 * close, the wheel turning with the seconds before it stops; at the result,
 * the number and its color, then the gain or the loss of the player.
 */
constexpr const char* TABLE_CLOSED_LINE = "THE TABLE IS CLOSED";
constexpr const char* NO_CHIPS_LINE = "NO CHIPS - SEE THE CASHIER";
constexpr const char* BETS_CLOSE_IN_START = "PLACE YOUR BETS - CLOSE IN ";
constexpr const char* RESULT_IN_START = "NO MORE BETS - RESULT IN ";
constexpr const char* NO_MORE_BETS_LINE = "NO MORE BETS";
constexpr const char* RESULT_GAP = " ";
constexpr const char* GAIN_GAP = "  ";
constexpr const char* GAIN_SIGN = "+";
constexpr const char* LOSS_SIGN = "-";
constexpr const char* RED_WORD = "RED";
constexpr const char* BLACK_WORD = "BLACK";
constexpr const char* GREEN_WORD = "GREEN";

/** @brief What the status line says while the bets are open when the master
 * refused the newest bets or the seat, by CasinoRefusal. */
constexpr const char* BETS_CLOSED_LINE = "TOO LATE - THE BETS ARE CLOSED";
constexpr const char* NOT_SEATED_LINE = "NO SEAT AT THIS TABLE YET";
constexpr const char* RULE_BROKEN_LINE = "THE TABLE REFUSES THESE BETS";
constexpr const char* NOT_ENOUGH_CHIPS_LINE =
    "NOT ENOUGH CHIPS - SEE THE CASHIER";
constexpr const char* ROOM_FULL_LINE = "THE TABLE IS FULL";
constexpr const char* SIGNED_OUT_LINE = "SIGN IN TO PLAY";
constexpr const char* CASINO_REFUSED_LINE = "THE CASINO REFUSED THE BETS";
constexpr const char* CASINO_SILENT_LINE = "THE CASINO DOES NOT ANSWER";

/**
 * @brief The players seated in the room, in a gold panel its name tags: a row
 * per player, ROSTER_ROW_HEIGHT apart from ROSTER_FIRST_ROW, its color in a
 * square, its name, then its stake, or its gain once the number is drawn, on
 * the right. The player comes first, and the panel lists ROSTER_MAXIMUM_ROWS
 * at most, as many as fit above the table: (78 - 10 - 18) / 11, so 4.
 */
constexpr sf::Vector2i ROSTER_TOP_LEFT(8, 10);
constexpr int ROSTER_WIDTH = 176;
constexpr int ROSTER_EMPTY_HEIGHT = 18;
constexpr int ROSTER_FIRST_ROW = 11;
constexpr int ROSTER_ROW_HEIGHT = 11;
constexpr sf::Vector2i ROSTER_TAG_OFFSET(8, -6);
constexpr int ROSTER_SQUARE_LEFT = 8;
constexpr int ROSTER_SQUARE_SIZE = 6;
constexpr int ROSTER_NAME_LEFT = 18;
constexpr int ROSTER_VALUE_MARGIN = 8;
constexpr const char* OWN_NAME_END = " (YOU)";
constexpr std::size_t ROSTER_MAXIMUM_ROWS = static_cast<std::size_t>(
    (ROULETTE_TABLE_TOP_LEFT.y - ROSTER_TOP_LEFT.y - ROSTER_EMPTY_HEIGHT) /
    ROSTER_ROW_HEIGHT);

/** @brief The plaque of the number drawn, under the wheel: its box, its
 * frames, the number twice as large, then its color and WINS beside it. */
constexpr sf::IntRect RESULT_PLAQUE_AREA({156, 190}, {44, 22});
constexpr int RESULT_PLAQUE_INNER_EDGE = 2;
constexpr int RESULT_NUMBER_TOP = 3;
constexpr int RESULT_NUMBER_SCALE = 2;
constexpr sf::Vector2i RESULT_COLOR_WORD(206, 193);
constexpr sf::Vector2i RESULT_WINS_WORD(206, 204);
constexpr const char* WINS_WORD = "WINS";

/** @brief The numbers drawn, newest first, LAST_NUMBERS_KEPT at most: LAST,
 * then a box per number, LAST_NUMBER_STEP apart from LAST_FIRST_BOX, the
 * newest framed in gold. */
constexpr std::size_t LAST_NUMBERS_KEPT = 7;
constexpr sf::Vector2i LAST_CAPTION(300, 200);
constexpr const char* LAST_CAPTION_TEXT = "LAST";
constexpr sf::Vector2i LAST_FIRST_BOX(330, 198);
constexpr sf::Vector2i LAST_BOX_SIZE(13, 11);
constexpr int LAST_NUMBER_STEP = 15;
constexpr int LAST_NUMBER_TOP = 2;

/** @brief The height of the digits of the cloth, which stand in the middle
 * of their box, one pixel right and one down. */
constexpr int CLOTH_NUMBER_HEIGHT = 8;

}  // namespace rtype::client
