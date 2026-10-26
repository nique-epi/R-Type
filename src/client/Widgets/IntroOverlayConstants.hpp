#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "IntroTimelineConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief The bands of the cinema format, INTRO_BAND_HEIGHT pixels at the top
 * and at the bottom, open in INTRO_BAND_OPENING from the slam; texts lie on
 * the bottom band from INTRO_BAND_TEXT_TOP, a name underlined
 * INTRO_UNDERLINE_GAP rows lower. */
constexpr int INTRO_BAND_HEIGHT = 24;
constexpr float INTRO_BAND_OPENING = 0.25F;
constexpr int INTRO_BAND_TEXT_TOP = 254;
constexpr int INTRO_UNDERLINE_GAP = 10;

/** @brief The credit, typed during the first beat: one letter every
 * INTRO_CREDIT_LETTER_TIME from INTRO_CREDIT_START, then erased letter by
 * letter over INTRO_CREDIT_ERASE_TIME, ending INTRO_CREDIT_ERASE_TIME before
 * the first beat. */
constexpr std::string_view INTRO_CREDIT = "R-TYPE TEAM PRESENTS";
constexpr float INTRO_CREDIT_START = 0.05F;
constexpr float INTRO_CREDIT_LETTER_TIME = 0.03F;
constexpr float INTRO_CREDIT_ERASE_TIME = 0.2F;
constexpr float INTRO_CREDIT_ERASE_START = INTRO_BEAT - INTRO_CREDIT_ERASE_TIME;
constexpr TextStyle INTRO_CREDIT_STYLE{.font = TextFont::Small,
                                       .color = PALE_BLUE};

/** @brief The nicknames of the four pilots, provisional, in the order of the
 * player colors: each typed during its close-up, one letter every
 * INTRO_NAME_LETTER_TIME from INTRO_NAME_DELAY after its beat. */
constexpr std::array INTRO_PILOT_NAMES{
    std::string_view("LOANN"), std::string_view("LUKAS"),
    std::string_view("NOLAN"), std::string_view("YANNIS")};
constexpr float INTRO_NAME_DELAY = 0.05F;
constexpr float INTRO_NAME_LETTER_TIME = 0.035F;

/** @brief PRESS ANY KEY, from INTRO_HINT_START, the middle of the fade of
 * the mockup (1 to 1.2 s), right-aligned at the safe margin. */
constexpr std::string_view INTRO_HINT = "PRESS ANY KEY";
constexpr float INTRO_HINT_START = 1.1F;
constexpr TextStyle INTRO_HINT_STYLE{.font = TextFont::Small,
                                     .color = PALE_BLUE,
                                     .alignment = TextAlignment::Right};

/**
 * @brief Where the logo lands: where MenuBuilder lays the logo of a page with
 * a version line (MenuBuilder::placeColumn), the place the title screen
 * shows it at. Three copies trail it, INTRO_TRAIL_SPACING pixels apart at
 * the start, in INTRO_LOGO_TRAIL; once it lands, two lines reach out of its
 * sides in INTRO_LINES_REACH_TIME and show for INTRO_LINES_SHOWN, the middle
 * of their fade in the mockup.
 */
constexpr sf::Vector2i INTRO_LOGO_PLACE(SAFE_MARGIN, SAFE_MARGIN +
                                                         TEXT_LINE_HEIGHT +
                                                         SPACE_BELOW_VERSION);
constexpr std::array INTRO_LOGO_TRAIL{DEEP_BLUE, BLUE, EDGE_BLUE};
constexpr float INTRO_TRAIL_SPACING = 22.0F;
constexpr float INTRO_LINES_REACH_TIME = 0.12F;
constexpr float INTRO_LINES_SHOWN = 0.225F;

}  // namespace rtype::client
