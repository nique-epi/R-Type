#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief HEADPHONES RECOMMENDED fades in from the ink of the intro, stays,
 * fades back out, then leaves the ink alone a moment before the next screen:
 * 3.6 s in all. */
constexpr std::chrono::milliseconds HEADPHONES_FADE_IN{600};
constexpr std::chrono::milliseconds HEADPHONES_HOLD{2000};
constexpr std::chrono::milliseconds HEADPHONES_FADE_OUT{600};
constexpr std::chrono::milliseconds HEADPHONES_INK_BEFORE_NEXT{400};

/** @brief The line under the logo, and the small line under it. */
constexpr std::string_view HEADPHONES_LINE = "HEADPHONES RECOMMENDED";
constexpr std::string_view HEADPHONES_SMALL_LINE = "FOR THE BEST EXPERIENCE";

/** @brief The letters of HEADPHONES_LOGO and the color each one paints; any
 * other letter, '.', leaves its pixel clear. */
constexpr char HEADPHONES_INK_LETTER = 'K';
constexpr char HEADPHONES_CYAN_LETTER = 'C';
constexpr char HEADPHONES_EDGE_BLUE_LETTER = 'E';
constexpr char HEADPHONES_DEEP_BLUE_LETTER = 'D';
constexpr char HEADPHONES_BLUE_LETTER = 'B';
constexpr char HEADPHONES_PALE_BLUE_LETTER = 'P';

/**
 * @brief The logo of a pair of headphones, one letter per pixel of the
 * interface: an arched band and two cups, outlined in ink, lit from above.
 */
constexpr std::array<std::string_view, 30> HEADPHONES_LOGO{
    "..............KKKKKKKK..............",
    "............KKCCCCCCCCKK............",
    "..........KKCCCEEEEEECCCKK..........",
    ".........KCCEEEDDDDDDEEECCK.........",
    "........KCCEDDDKKKKKKDDDECCK........",
    ".......KCEEDKKK......KKKDEECK.......",
    "......KCEEDK............KDEECK......",
    ".....KCCEDK..............KDECCK.....",
    ".....KCEDK................KDECK.....",
    "....KCEDK..................KDECK....",
    "....KCEDK..................KDECK....",
    "...KCCEDK..................KDECCK...",
    "...KCEDK....................KDECK...",
    "...KCEDK....................KDECK...",
    "...KCEDK....................KDECK...",
    "...KCEEK....................KEECK...",
    "..KKCEEK....................KEECKK..",
    ".KDCCCPPK..................KPPCCCDK.",
    "KDDCCCPPPK................KPPPCCCDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDEEEBBBK................KBBBEEEDDK",
    "KDDDDDDDDK................KDDDDDDDDK",
    ".KDDDDDDK..................KDDDDDDK.",
    "..KKKKKK....................KKKKKK.."};

/** @brief The logo, HEADPHONES_LOGO_GAP pixels, the line,
 * HEADPHONES_LINE_GAP pixels, then the small line: one block centered on
 * INTERFACE_AREA, 30 + 12 + 8 + 6 + 8 = 64 pixels tall. */
constexpr int HEADPHONES_LOGO_GAP = 12;
constexpr int HEADPHONES_LINE_GAP = 6;
constexpr sf::Vector2i HEADPHONES_LOGO_SIZE{
    static_cast<int>(HEADPHONES_LOGO.front().size()),
    static_cast<int>(HEADPHONES_LOGO.size())};
constexpr int HEADPHONES_BLOCK_HEIGHT = HEADPHONES_LOGO_SIZE.y +
                                        HEADPHONES_LOGO_GAP + TEXT_LINE_HEIGHT +
                                        HEADPHONES_LINE_GAP + TEXT_LINE_HEIGHT;
constexpr sf::Vector2i HEADPHONES_LOGO_TOP_LEFT{
    (INTERFACE_AREA.size.x - HEADPHONES_LOGO_SIZE.x) / SIDES_PER_AXIS,
    (INTERFACE_AREA.size.y - HEADPHONES_BLOCK_HEIGHT) / SIDES_PER_AXIS};
constexpr sf::Vector2i HEADPHONES_LINE_ANCHOR{
    INTERFACE_AREA.size.x / SIDES_PER_AXIS,
    HEADPHONES_LOGO_TOP_LEFT.y + HEADPHONES_LOGO_SIZE.y + HEADPHONES_LOGO_GAP};
constexpr sf::Vector2i HEADPHONES_SMALL_LINE_ANCHOR{
    HEADPHONES_LINE_ANCHOR.x,
    HEADPHONES_LINE_ANCHOR.y + TEXT_LINE_HEIGHT + HEADPHONES_LINE_GAP};

/** @brief The line in the cyan of the HUD, the small line in pale blue, both
 * outlined in ink like every text laid on the sky. */
constexpr TextStyle HEADPHONES_LINE_STYLE{.font = TextFont::Regular,
                                          .color = HUD_CYAN,
                                          .outlined = true,
                                          .alignment = TextAlignment::Center};
constexpr TextStyle HEADPHONES_SMALL_LINE_STYLE{
    .font = TextFont::Small,
    .color = PALE_BLUE,
    .outlined = true,
    .alignment = TextAlignment::Center};

}  // namespace rtype::client
