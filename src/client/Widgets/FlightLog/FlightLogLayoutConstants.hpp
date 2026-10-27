#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstdint>
#include "DustGrain.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "InterfaceConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

/** @brief A half of the book, cover and pages, as the sheet draws it, and the
 * page it holds, PAGE_INSET from the hinge and the top of the half. */
constexpr sf::Vector2i BOOK_HALF_SIZE(COVER_FRONT_FRAME.width,
                                      COVER_FRONT_FRAME.height);
constexpr sf::Vector2i PAGE_SIZE(200, 196);
constexpr sf::Vector2i PAGE_INSET(2, 3);

/** @brief Where the book lies: its top row, and the column of its hinge,
 * the middle of the picture once open, its left edge while closed so the
 * closed book lies in the middle too. */
constexpr int BOOK_TOP = 12;
constexpr int OPEN_HINGE = INTERFACE_AREA.size.x / SIDES_PER_AXIS;
constexpr int CLOSED_HINGE = OPEN_HINGE - (BOOK_HALF_SIZE.x / SIDES_PER_AXIS);

/** @brief The closed book shows its pages and back cover BOOK_SIDE_DEPTH
 * pixels deep at its right and bottom, striped every PAGE_STRIPE_PERIOD; its
 * shadow falls BOOK_SHADOW_OFFSET away. */
constexpr int BOOK_SIDE_DEPTH = 4;
constexpr int PAGE_STRIPE_PERIOD = 2;
constexpr sf::Vector2i BOOK_SHADOW_OFFSET(6, 6);

/** @brief The free edge of a turning cover rises and sinks by COVER_LIFT
 * rows at most; the open pages meet over a spine SPINE_WIDTH wide. */
constexpr int COVER_LIFT = 8;
constexpr int SPINE_WIDTH = 2;

/** @brief Rows the book drops from; rows it falls by after QUIT, columns it
 * slides by after PLAY AGAIN; rows the label falls by. */
constexpr int DROP_HEIGHT = 280;
constexpr int EXIT_FALL = 320;
constexpr int EXIT_SLIDE = 400;
constexpr int LABEL_FALL = 240;

/** @brief The label of the taunt on the cover, its two pieces of tape, and
 * the rows between two lines of the taunt. */
constexpr sf::Vector2i LABEL_ON_COVER(34, 88);
constexpr sf::Vector2i LABEL_SIZE(TAUNT_LABEL_FRAME.width,
                                  TAUNT_LABEL_FRAME.height);
constexpr std::array LABEL_TAPES{sf::IntRect({-6, -3}, {18, 6}),
                                 sf::IntRect({LABEL_SIZE.x - 12, -3}, {18, 6})};
constexpr int TAUNT_LINE_PITCH = 12;

/** @brief The left page, from its top-left corner: MISSION LOG, the stamp,
 * ALL PILOTS DOWN, the team score at TITLE_SCALE, the polaroids. */
constexpr sf::Vector2i MISSION_LOG_TOP_LEFT(12, 10);
constexpr sf::IntRect STAMP_BOX({22, 24}, {156, 28});
constexpr int STAMP_FRAME_GAP = 2;
constexpr int TITLE_SCALE = 2;
constexpr std::array STAMP_SLAM_SCALES{4, 3};
constexpr int STAMP_HOLE_COUNT = 70;
constexpr std::uint32_t STAMP_HOLES_SEED = 42;
constexpr int DOWN_LINE_TOP = 62;
constexpr int TEAM_LABEL_TOP = 82;
constexpr sf::Vector2i TEAM_SCORE_END(156, 94);
constexpr int POLAROID_TOP = 128;
constexpr sf::Vector2i POLAROID_SIZE(42, 46);
constexpr int POLAROID_GAP = 4;
constexpr sf::IntRect POLAROID_WINDOW({3, 3}, {36, 26});
constexpr std::array POLAROID_STARS{sf::Vector2i(10, 7), sf::Vector2i(33, 24),
                                    sf::Vector2i(28, 6)};
constexpr sf::Vector2i POLAROID_SHIP(5, 8);
constexpr sf::Vector2i POLAROID_MARKER(5, 34);
constexpr sf::Vector2i POLAROID_CAPTION(15, 34);
constexpr sf::IntRect POLAROID_TAPE({14, -3}, {14, 6});
constexpr int POLAROID_DROP = 6;

/** @brief The right page, from its top-left corner: the heading and its
 * rule, then a row per pilot from FIRST_ROW_TOP, RANKING_ROW_PITCH apart. */
constexpr sf::Vector2i RANKING_HEADING_TOP_LEFT(28, 12);
constexpr sf::Vector2i RANKING_RULE_TOP_LEFT(28, 24);
constexpr int RANKING_LINE_WIDTH = 164;
constexpr int FIRST_ROW_TOP = 34;
constexpr int RANKING_ROW_PITCH = 38;

/** @brief A row, from its top-left corner: the rank, the name, the marker,
 * YOU on ink, the dotted leader, the score, the separator, the medal. While
 * it slams in, a digit moves up and left by SLAM_SHIFT per scale above
 * TITLE_SCALE, so it lands where it stays. */
constexpr sf::Vector2i RANK_DIGIT_OFFSET(28, 2);
constexpr int SLAM_SHIFT = 4;
constexpr sf::Vector2i PILOT_NAME_OFFSET(48, 1);
constexpr sf::Vector2i PILOT_MARKER_OFFSET(48, 15);
constexpr sf::Vector2i YOU_TAG_OFFSET(59, 14);
constexpr int YOU_TAG_HEIGHT = 9;
constexpr int YOU_TAG_PADDING = 5;
constexpr sf::Vector2i YOU_TEXT_OFFSET(3, 1);
constexpr int LEADER_LEFT = 60;
constexpr int LEADER_RIGHT = 136;
constexpr int LEADER_ROW = 21;
constexpr int LEADER_AFTER_TAG = 3;
constexpr sf::Vector2i ROW_SCORE_END(196, 15);
constexpr int ROW_SEPARATOR_ROW = 30;
constexpr sf::Vector2i MEDAL_OFFSET(182, -2);
constexpr int MEDAL_DROP = 8;
/** @brief The rank whose rows get the medal: every pilot tied first. */
constexpr int MEDAL_RANK = 1;

/** @brief The ribbon hanging under the open book, from its hinge and top:
 * a strip, its shaded column, its forked end and the notch between the two
 * tails of the fork. */
constexpr sf::Vector2i RIBBON_OFFSET(118, BOOK_HALF_SIZE.y - 2);
constexpr sf::Vector2i RIBBON_SIZE(5, 13);
constexpr sf::IntRect RIBBON_NOTCH({2, 12}, {1, 1});
constexpr std::array RIBBON_FORK{sf::IntRect({1, 13}, {1, 2}),
                                 sf::IntRect({4, 13}, {1, 2})};

/** @brief The dust of the landing, from the bottom corners of the closed
 * book: big grains first, then small, every other one gone at the end. */
constexpr std::array DUST_GRAINS{
    DustGrain{.side = -1, .rise = 4, .reach = 12, .row = 0},
    DustGrain{.side = 1, .rise = 6, .reach = 16, .row = 1},
    DustGrain{.side = -1, .rise = 9, .reach = 22, .row = 2},
    DustGrain{.side = 1, .rise = 3, .reach = 11, .row = 0},
    DustGrain{.side = -1, .rise = 7, .reach = 25, .row = 3},
    DustGrain{.side = 1, .rise = 10, .reach = 19, .row = 1},
    DustGrain{.side = -1, .rise = 5, .reach = 14, .row = 2},
    DustGrain{.side = 1, .rise = 8, .reach = 27, .row = 0},
    DustGrain{.side = -1, .rise = 11, .reach = 18, .row = 1},
    DustGrain{.side = 1, .rise = 4, .reach = 23, .row = 3},
    DustGrain{.side = -1, .rise = 6, .reach = 28, .row = 0},
    DustGrain{.side = 1, .rise = 9, .reach = 13, .row = 2}};
constexpr int LARGE_GRAIN = 2;
constexpr float GRAIN_SHRINK_SHARE = 0.5F;
constexpr float GRAIN_THIN_SHARE = 0.8F;

/** @brief The shake of the stamp: the whole book, by this much, one way then
 * the other every SHAKE_STEP, so SHAKE_SIDES steps make a back and forth. */
constexpr sf::Vector2i SHAKE_OFFSET(2, 1);
constexpr std::int64_t SHAKE_SIDES = 2;

/** @brief The call to play again, centered on PROMPT_TOP, with a glow of
 * PROMPT_GLOW pixels; the pointer presses it over PROMPT_AREA. QUIT is a
 * small button under the bottom-right corner of the open book. */
constexpr int PROMPT_TOP = 244;
constexpr int PROMPT_WIDTH =
    static_cast<int>(PROMPT_TEXT.size()) * REGULAR_FONT_ADVANCE;
constexpr int PROMPT_LEFT =
    (INTERFACE_AREA.size.x - PROMPT_WIDTH) / SIDES_PER_AXIS;
constexpr sf::Vector2i PROMPT_HIT_MARGIN(6, 4);
constexpr sf::IntRect PROMPT_AREA(
    {PROMPT_LEFT - PROMPT_HIT_MARGIN.x, PROMPT_TOP - PROMPT_HIT_MARGIN.y},
    {PROMPT_WIDTH + (SIDES_PER_AXIS * PROMPT_HIT_MARGIN.x),
     TEXT_LINE_HEIGHT + (SIDES_PER_AXIS * PROMPT_HIT_MARGIN.y)});
constexpr int PROMPT_GLOW = 2;
constexpr sf::Vector2i QUIT_SIZE(54, 14);
constexpr sf::IntRect QUIT_BUTTON({OPEN_HINGE + BOOK_HALF_SIZE.x - QUIT_SIZE.x,
                                   224},
                                  QUIT_SIZE);
constexpr sf::Vector2i QUIT_CROSS_OFFSET(5, 3);
constexpr sf::Vector2i QUIT_LABEL_OFFSET(16, 3);

}  // namespace rtype::client
