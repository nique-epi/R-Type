#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "CallArea.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TestCardPart.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/**
 * @brief An old screen turning off: the picture is squeezed toward its middle
 * row in SCREEN_OFF_COLLAPSE, easing out; once it covers SCREEN_OFF_LINE_ROWS
 * rows or fewer, it becomes a line SCREEN_OFF_LINE_THICKNESS rows thick across
 * the whole picture, which closes into the middle in SCREEN_OFF_CLOSE, easing
 * out; the screen then stays dark SCREEN_OFF_DARK. The card shows
 * SCREEN_OFF_TIME after the client gave up. The line is pale blue, never
 * white, so nothing flashes.
 */
constexpr engine::Duration SCREEN_OFF_COLLAPSE = std::chrono::milliseconds(200);
constexpr engine::Duration SCREEN_OFF_CLOSE = std::chrono::milliseconds(150);
constexpr engine::Duration SCREEN_OFF_DARK = std::chrono::milliseconds(100);
constexpr engine::Duration SCREEN_OFF_TIME =
    SCREEN_OFF_COLLAPSE + SCREEN_OFF_CLOSE + SCREEN_OFF_DARK;
constexpr int SCREEN_OFF_LINE_ROWS = 3;
constexpr int SCREEN_OFF_LINE_THICKNESS = 2;
constexpr sf::Color SCREEN_OFF_LINE_COLOR = PALE_BLUE;
constexpr sf::Color SCREEN_OFF_DARK_COLOR = SKY_COLOR;

/**
 * @brief The colors of the SMPTE color bars (ECR 1-1978), as the Wikimedia
 * drawing of the pattern gives them: seven bars at 75 % (gray, yellow, cyan,
 * green, magenta, red, blue), the -I dark blue, the white at 100 %, the +Q
 * purple, the black at 7.5 %, and the two blacks of the PLUGE, 4 % under and
 * over it. The only colors of the client outside its palette: the card is
 * the real one.
 */
constexpr sf::Color TEST_CARD_GRAY(192, 192, 192);
constexpr sf::Color TEST_CARD_YELLOW(192, 192, 0);
constexpr sf::Color TEST_CARD_CYAN(0, 192, 192);
constexpr sf::Color TEST_CARD_GREEN(0, 192, 0);
constexpr sf::Color TEST_CARD_MAGENTA(192, 0, 192);
constexpr sf::Color TEST_CARD_RED(192, 0, 0);
constexpr sf::Color TEST_CARD_BLUE(0, 0, 192);
constexpr sf::Color TEST_CARD_DARK_BLUE(0, 33, 76);
constexpr sf::Color TEST_CARD_WHITE(255, 255, 255);
constexpr sf::Color TEST_CARD_PURPLE(50, 0, 106);
constexpr sf::Color TEST_CARD_BLACK(19, 19, 19);
constexpr sf::Color TEST_CARD_DARKER_BLACK(9, 9, 9);
constexpr sf::Color TEST_CARD_LIGHTER_BLACK(29, 29, 29);

/**
 * @brief The rows of the test card over the whole picture: the pattern splits
 * its height into TEST_CARD_HEIGHT_PARTS, TEST_CARD_BAR_PARTS for the bars and
 * TEST_CARD_CASTELLATION_PARTS for the castellations, the bottom row taking
 * the rest; and each row into the twelfths of a bar, TEST_CARD_ROW_TWELFTHS
 * across. A whole bar is TEST_CARD_BAR_WIDTH twelfths, -I, white, +Q and the
 * black after them TEST_CARD_WIDE_WIDTH (5/4 of a bar), a PLUGE bar
 * TEST_CARD_PLUGE_WIDTH (a third of a bar).
 */
constexpr int TEST_CARD_HEIGHT_PARTS = 12;
constexpr int TEST_CARD_BAR_PARTS = 8;
constexpr int TEST_CARD_CASTELLATION_PARTS = 1;
constexpr int TEST_CARD_BAR_ROWS =
    INTERFACE_AREA.size.y * TEST_CARD_BAR_PARTS / TEST_CARD_HEIGHT_PARTS;
constexpr int TEST_CARD_CASTELLATION_ROWS = INTERFACE_AREA.size.y *
                                            TEST_CARD_CASTELLATION_PARTS /
                                            TEST_CARD_HEIGHT_PARTS;
constexpr int TEST_CARD_BOTTOM_TOP =
    TEST_CARD_BAR_ROWS + TEST_CARD_CASTELLATION_ROWS;
constexpr int TEST_CARD_BAR_WIDTH = 12;
constexpr int TEST_CARD_WIDE_WIDTH = 15;
constexpr int TEST_CARD_PLUGE_WIDTH = 4;

constexpr std::array TEST_CARD_BARS{
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_GRAY},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_YELLOW},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_CYAN},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_GREEN},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_MAGENTA},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_RED},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLUE}};
constexpr std::array TEST_CARD_CASTELLATIONS{
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLUE},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLACK},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_MAGENTA},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLACK},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_CYAN},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLACK},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_GRAY}};
constexpr std::array TEST_CARD_BOTTOM_ROW{
    TestCardPart{.twelfths = TEST_CARD_WIDE_WIDTH,
                 .color = TEST_CARD_DARK_BLUE},
    TestCardPart{.twelfths = TEST_CARD_WIDE_WIDTH, .color = TEST_CARD_WHITE},
    TestCardPart{.twelfths = TEST_CARD_WIDE_WIDTH, .color = TEST_CARD_PURPLE},
    TestCardPart{.twelfths = TEST_CARD_WIDE_WIDTH, .color = TEST_CARD_BLACK},
    TestCardPart{.twelfths = TEST_CARD_PLUGE_WIDTH,
                 .color = TEST_CARD_DARKER_BLACK},
    TestCardPart{.twelfths = TEST_CARD_PLUGE_WIDTH, .color = TEST_CARD_BLACK},
    TestCardPart{.twelfths = TEST_CARD_PLUGE_WIDTH,
                 .color = TEST_CARD_LIGHTER_BLACK},
    TestCardPart{.twelfths = TEST_CARD_BAR_WIDTH, .color = TEST_CARD_BLACK}};
constexpr int TEST_CARD_ROW_TWELFTHS =
    static_cast<int>(TEST_CARD_BARS.size()) * TEST_CARD_BAR_WIDTH;

/**
 * @brief The box in the middle of the bars: TEST_CARD_BOX_SIZE, black with an
 * ink outline around it; NO SIGNAL written TEST_CARD_TITLE_DROP under its top,
 * in star white, the reason TEST_CARD_REASON_DROP under it, in small pale
 * blue, both centered on the picture.
 */
constexpr sf::Vector2i TEST_CARD_BOX_SIZE(224, 68);
constexpr sf::IntRect TEST_CARD_BOX(
    {(INTERFACE_AREA.size.x - TEST_CARD_BOX_SIZE.x) / SIDES_PER_AXIS,
     (TEST_CARD_BAR_ROWS - TEST_CARD_BOX_SIZE.y) / SIDES_PER_AXIS},
    TEST_CARD_BOX_SIZE);
constexpr sf::IntRect TEST_CARD_BOX_OUTLINE(
    TEST_CARD_BOX.position - sf::Vector2i(BORDER_THICKNESS, BORDER_THICKNESS),
    TEST_CARD_BOX.size + sf::Vector2i((SIDES_PER_AXIS * BORDER_THICKNESS),
                                      (SIDES_PER_AXIS * BORDER_THICKNESS)));
constexpr int TEST_CARD_TITLE_DROP = 12;
constexpr int TEST_CARD_REASON_DROP = 27;
constexpr sf::Color TEST_CARD_BOX_COLOR = TEST_CARD_BLACK;
constexpr sf::Color TEST_CARD_OUTLINE_COLOR = INK;
constexpr sf::Color NO_SIGNAL_TITLE_COLOR = STAR_WHITE;
constexpr sf::Color NO_SIGNAL_REASON_COLOR = PALE_BLUE;
constexpr std::string_view NO_SIGNAL_TEXT = "NO SIGNAL";
constexpr std::string_view SILENT_SERVER_TEXT = "THE SERVER STOPPED ANSWERING.";
constexpr std::string_view CLOSED_SERVER_TEXT = "THE SERVER SHUT DOWN.";

/**
 * @brief The call to reconnect, written as every call (GlowingCallConstants),
 * RECONNECT_CALL_DELAY after the card shows, centered on the picture,
 * TEST_CARD_CALL_DROP under the top of the box; the pointer presses it over
 * RECONNECT_CALL_AREA.
 */
constexpr std::string_view RECONNECT_CALL_TEXT = "PRESS ENTER TO RECONNECT";
constexpr engine::Duration RECONNECT_CALL_DELAY =
    std::chrono::milliseconds(200);
constexpr int TEST_CARD_CALL_DROP = 46;
constexpr int RECONNECT_CALL_WIDTH =
    static_cast<int>(RECONNECT_CALL_TEXT.size()) * REGULAR_FONT_ADVANCE;
constexpr sf::Vector2i RECONNECT_CALL_TOP_LEFT(
    (INTERFACE_AREA.size.x - RECONNECT_CALL_WIDTH) / SIDES_PER_AXIS,
    TEST_CARD_BOX.position.y + TEST_CARD_CALL_DROP);
constexpr sf::IntRect RECONNECT_CALL_AREA =
    callArea(RECONNECT_CALL_TOP_LEFT, RECONNECT_CALL_WIDTH);

}  // namespace rtype::client
