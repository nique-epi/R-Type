#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "CasinoBar.hpp"
#include "LetterCounts.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
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
constexpr Translation SPIN_IN_LINE{.english = "SPIN IN {SECONDS}",
                                   .french = "TIRAGE {SECONDS}"};
constexpr std::string_view ROULETTE_SECONDS_MARK = "{SECONDS}";
constexpr engine::Duration SPIN_COUNTDOWN_SPAN = std::chrono::seconds(20);
constexpr std::int64_t SPIN_HURRY_SECONDS = 3;

/**
 * @brief The banner of a big win, across the middle of the table for the
 * first BIG_WIN_TIME of the result: JACKPOT twice the size of a text in
 * gold, then who won how many chips, in a gold frame lined in red. A win is
 * big once the payout of a player passes its stake by BIG_WIN_GAIN chips,
 * more than four times the dearest look of the shop: 100 chips on a number
 * gain 3500 and show it, 50 gain 1750 and do not.
 */
constexpr std::uint32_t BIG_WIN_GAIN = 2500;
constexpr engine::Duration BIG_WIN_TIME = std::chrono::seconds(4);
constexpr sf::IntRect BIG_WIN_AREA({60, 104}, {360, 62});
constexpr int BIG_WIN_LINING_INSET = 3;
constexpr sf::Vector2i BIG_WIN_TITLE_TOP_MIDDLE(240, 114);
constexpr int BIG_WIN_TITLE_SCALE = 2;
constexpr sf::Vector2i BIG_WIN_LINE_TOP_MIDDLE(240, 142);
constexpr Translation BIG_WIN_TITLE{.english = "JACKPOT", .french = "JACKPOT"};
constexpr Translation BIG_WIN_LINE{.english = "{PILOT} WON {GAIN} CHIPS",
                                   .french = "{PILOT} GAGNE {GAIN} JETONS"};
constexpr std::string_view BIG_WIN_PILOT_MARK = "{PILOT}";
constexpr std::string_view BIG_WIN_GAIN_MARK = "{GAIN}";

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
constexpr Translation BAND_STAKE_CAPTION_TEXT{.english = "STAKE",
                                              .french = "MISE"};

/** @brief The bars of the band: CLEAR and REBET side by side, BACK under
 * REBET. */
constexpr CasinoBar CLEAR_BAR{.label = {.english = "CLEAR", .french = "VIDER"},
                              .keyName = "C",
                              .area = sf::IntRect({312, 234}, {78, 16}),
                              .key = sf::Keyboard::Key::C};
constexpr CasinoBar REBET_BAR{.label = {.english = "REBET", .french = "IDEM"},
                              .keyName = "R",
                              .area = sf::IntRect({396, 234}, {78, 16}),
                              .key = sf::Keyboard::Key::R};
constexpr CasinoBar TABLE_BACK_BAR{.label = BACK_LABEL,
                                   .keyName = CANCEL_KEY_NAME,
                                   .area = sf::IntRect({376, 253}, {98, 14}),
                                   .key = MENU_CANCEL_KEY};

/**
 * @brief What the status line says: the table closed before the first state
 * of the room, no chip held, the bets open with the seconds before they
 * close, the wheel turning with the seconds before it stops; at the result,
 * the number and its color, then the gain or the loss of the player.
 */
constexpr Translation TABLE_CLOSED_LINE{.english = "THE TABLE IS CLOSED",
                                        .french = "LA TABLE EST FERMÉE"};
constexpr Translation NO_CHIPS_LINE{.english = "NO CHIPS - SEE THE CASHIER",
                                    .french = "AUCUN JETON - VOIR LA CAISSE"};
constexpr Translation BETS_CLOSE_IN_LINE{
    .english = "PLACE YOUR BETS - CLOSE IN {SECONDS}",
    .french = "FAITES VOS JEUX - FIN DANS {SECONDS}"};
constexpr Translation RESULT_IN_LINE{
    .english = "NO MORE BETS - RESULT IN {SECONDS}",
    .french = "RIEN NE VA PLUS - RÉSULTAT DANS {SECONDS}"};
constexpr Translation NO_MORE_BETS_LINE{.english = "NO MORE BETS",
                                        .french = "RIEN NE VA PLUS"};
constexpr const char* RESULT_GAP = " ";
constexpr const char* GAIN_GAP = "  ";
constexpr const char* GAIN_SIGN = "+";
constexpr const char* LOSS_SIGN = "-";
constexpr Translation RED_WORD{.english = "RED", .french = "ROUGE"};
constexpr Translation BLACK_WORD{.english = "BLACK", .french = "NOIR"};
constexpr Translation GREEN_WORD{.english = "GREEN", .french = "VERT"};

/** @brief What the status line says while the bets are open when the master
 * refused the newest bets or the seat, by CasinoRefusal. */
constexpr Translation BETS_CLOSED_LINE{
    .english = "TOO LATE - THE BETS ARE CLOSED",
    .french = "TROP TARD - LES JEUX SONT FAITS"};
constexpr Translation NOT_SEATED_LINE{
    .english = "NO SEAT AT THIS TABLE YET",
    .french = "PAS ENCORE DE PLACE À CETTE TABLE"};
constexpr Translation RULE_BROKEN_LINE{
    .english = "THE TABLE REFUSES THESE BETS",
    .french = "LA TABLE REFUSE CES MISES"};
constexpr Translation NOT_ENOUGH_CHIPS_LINE{
    .english = "NOT ENOUGH CHIPS - SEE THE CASHIER",
    .french = "MANQUE DE JETONS - VOIR LA CAISSE"};
constexpr Translation ROOM_FULL_LINE{.english = "THE TABLE IS FULL",
                                     .french = "LA TABLE EST PLEINE"};
constexpr Translation SIGNED_OUT_LINE{.english = "SIGN IN TO PLAY",
                                      .french = "CONNECTEZ-VOUS POUR JOUER"};
constexpr Translation CASINO_REFUSED_LINE{
    .english = "THE CASINO REFUSED THE BETS",
    .french = "LE CASINO A REFUSÉ LES MISES"};
constexpr Translation CASINO_SILENT_LINE{
    .english = "THE CASINO DOES NOT ANSWER",
    .french = "LE CASINO NE RÉPOND PAS"};

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
constexpr Translation OWN_NAME_END{.english = " (YOU)", .french = " (MOI)"};
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
constexpr Translation WINS_WORD{.english = "WINS", .french = "GAGNE"};

/** @brief The numbers drawn, newest first, LAST_NUMBERS_KEPT at most: LAST,
 * then a box per number, LAST_NUMBER_STEP apart from LAST_FIRST_BOX, the
 * newest framed in gold. */
constexpr std::size_t LAST_NUMBERS_KEPT = 7;
constexpr sf::Vector2i LAST_CAPTION(300, 200);
constexpr Translation LAST_CAPTION_TEXT{.english = "LAST", .french = "TIRÉS"};
constexpr sf::Vector2i LAST_FIRST_BOX(330, 198);
constexpr sf::Vector2i LAST_BOX_SIZE(13, 11);
constexpr int LAST_NUMBER_STEP = 15;
constexpr int LAST_NUMBER_TOP = 2;

/** @brief The height of the digits of the cloth, which stand in the middle
 * of their box, one pixel right and one down. */
constexpr int CLOTH_NUMBER_HEIGHT = 8;

static_assert(lettersIn(SPIN_IN_LINE.french) <= lettersIn(SPIN_IN_LINE.english),
              "the French countdown fits the plaque of the spin");

}  // namespace rtype::client
