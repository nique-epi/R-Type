#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "MaskArea.hpp"
#include "MaskPixel.hpp"
#include "MaskSpan.hpp"

/** @brief The character a test drawing marks a drawn pixel with. */
constexpr char DRAWN_TEST_PIXEL = '#';

/** @brief A mask of 3 by 2 pixels, small enough to name each corner. */
constexpr int SMALL_WIDTH = 3;
constexpr int SMALL_HEIGHT = 2;

/** @brief The middle pixel of the bottom row of a SMALL_WIDTH by SMALL_HEIGHT
 * mask, and its three neighbours inside it. */
constexpr rtype::engine::MaskPixel BOTTOM_MIDDLE{.x = 1, .y = 1};
constexpr rtype::engine::MaskPixel BOTTOM_LEFT{.x = 0, .y = 1};
constexpr rtype::engine::MaskPixel BOTTOM_RIGHT{.x = 2, .y = 1};
constexpr rtype::engine::MaskPixel TOP_MIDDLE{.x = 1, .y = 0};

/** @brief The four corners of a SMALL_WIDTH by SMALL_HEIGHT mask. */
constexpr std::array SMALL_CORNERS{rtype::engine::MaskPixel{.x = 0, .y = 0},
                                   rtype::engine::MaskPixel{.x = 2, .y = 0},
                                   rtype::engine::MaskPixel{.x = 0, .y = 1},
                                   rtype::engine::MaskPixel{.x = 2, .y = 1}};

/** @brief Pixels just outside a SMALL_WIDTH by SMALL_HEIGHT mask: one column
 * past its right edge, and one row above its top edge. */
constexpr rtype::engine::MaskPixel PAST_THE_RIGHT_EDGE{.x = 3, .y = 0};
constexpr rtype::engine::MaskPixel ABOVE_THE_TOP_EDGE{.x = 0, .y = -1};
constexpr rtype::engine::MaskPixel LEFT_OF_THE_LEFT_EDGE{.x = -1, .y = 0};

/** @brief A mask two words wide: a row of 100 pixels needs a second word of
 * 64 pixels. */
constexpr int WIDE_SIDE = 100;

/** @brief The last pixel of the first word of a row, and the first pixel of
 * the second word. */
constexpr rtype::engine::MaskPixel LAST_PIXEL_OF_THE_FIRST_WORD{.x = 63,
                                                                .y = 0};
constexpr rtype::engine::MaskPixel FIRST_PIXEL_OF_THE_SECOND_WORD{.x = 64,
                                                                  .y = 0};

/** @brief The first pixel of the second row of a WIDE_SIDE mask, which a
 * pixel kept in the wrong word would land on. */
constexpr rtype::engine::MaskPixel FIRST_PIXEL_OF_THE_SECOND_ROW{.x = 0,
                                                                 .y = 1};

/** @brief Four pixels from column 62 to 65 of the first row: the two last of
 * the first word and the two first of the second. With columns 63 and 64
 * drawn, they read 0110. */
constexpr rtype::engine::MaskSpan SPAN_ACROSS_TWO_WORDS{
    .row = 0, .first = 62, .count = 4};
constexpr std::uint64_t MIDDLE_TWO_OF_FOUR = 0b0110;

/** @brief A row drawn at both ends, read whole, then its two inner pixels. */
constexpr std::array<std::string_view, 1> ROW_DRAWN_AT_BOTH_ENDS{"#..#"};
constexpr rtype::engine::MaskSpan WHOLE_ROW_OF_FOUR{
    .row = 0, .first = 0, .count = 4};
constexpr std::uint64_t BOTH_ENDS_OF_FOUR = 0b1001;
constexpr rtype::engine::MaskSpan INNER_TWO_OF_FOUR{
    .row = 0, .first = 1, .count = 2};
constexpr std::uint64_t NO_PIXEL = 0;

/** @brief The largest side a mask may have, and the first it may not. */
constexpr int LARGEST_MASK_SIDE = 256;
constexpr int SIDE_PAST_THE_LARGEST = 257;

/** @brief Spans a SMALL_WIDTH mask refuses: one that covers no pixel, one
 * that covers one pixel more than a word, and one that runs one pixel past
 * the row. */
constexpr rtype::engine::MaskSpan SPAN_OF_NO_PIXEL{
    .row = 0, .first = 0, .count = 0};
constexpr rtype::engine::MaskSpan SPAN_PAST_A_WORD{
    .row = 0, .first = 0, .count = 65};
constexpr rtype::engine::MaskSpan SPAN_PAST_THE_ROW{
    .row = 0, .first = 2, .count = 2};

/** @brief The ship of entities.png, 32 by 16 pixels, as its frame draws it. */
constexpr std::array<std::string_view, 16> SHIP_ROWS{
    "................................", "..............#####.............",
    "............########............", "...........########.............",
    "..####################..........", ".#########################......",
    "##############################..", "###############################.",
    "###############################.", "##############################..",
    ".#########################......", "..####################..........",
    "...........########.............", "............########............",
    "..............#####.............", "................................"};

/** @brief The bullet of an enemy in entities.png, 8 by 8 pixels. */
constexpr std::array<std::string_view, 8> BULLET_ROWS{
    "...##...", "..####..", ".######.", "########",
    "########", ".######.", "..####..", "...##..."};

/** @brief Where a bullet lies, relative to the ship laid at 0, 0, when its
 * frame overlaps the frame of the ship over the empty corner past the top fin:
 * the frames overlap, no drawn pixel does. */
constexpr int BULLET_IN_THE_GAP_LEFT = 24;
constexpr int BULLET_IN_THE_GAP_TOP = -3;

/** @brief Where a bullet lies when it covers the tip of the right wing: its
 * pixel 1, 2 lies on the pixel 27, 6 of the ship. */
constexpr int BULLET_ON_THE_WING_LEFT = 26;
constexpr int BULLET_ON_THE_WING_TOP = 4;

/** @brief Two squares of 2 by 2 drawn pixels, laid side by side (the second
 * starts where the first ends) or sharing one column. */
constexpr int SQUARE_SIDE = 2;
constexpr int SQUARE_BESIDE_LEFT = 2;
constexpr int SQUARE_SHARING_A_COLUMN_LEFT = 1;

/** @brief A square laid below the other, sharing no row with it. */
constexpr int SQUARE_BELOW_TOP = 10;

/** @brief The one drawn pixel of a WIDE_SIDE row, in its second word. */
constexpr rtype::engine::MaskPixel PIXEL_IN_THE_SECOND_WORD{.x = 80, .y = 0};

/** @brief A SMALL_WIDTH by SMALL_HEIGHT mask with its top-left pixel alone
 * drawn. */
constexpr std::array<std::string_view, 2> TOP_LEFT_DRAWN_ROWS{"#..", "..."};

/** @brief The cell under the top-left pixel of a mask laid at 0, 0, and the
 * cells under every other pixel of a SMALL_WIDTH by SMALL_HEIGHT mask. */
constexpr rtype::engine::MaskArea TOP_LEFT_CELL{
    .left = 0, .top = 0, .width = 1, .height = 1};
constexpr rtype::engine::MaskArea ALL_BUT_THE_FIRST_COLUMN{
    .left = 1, .top = 0, .width = 2, .height = 2};

/** @brief The column just past the right edge of a SMALL_WIDTH mask laid at
 * 0, 0, and the row just above its top edge. */
constexpr rtype::engine::MaskArea COLUMN_PAST_THE_RIGHT_EDGE{
    .left = 3, .top = 0, .width = 1, .height = 2};
constexpr rtype::engine::MaskArea ROW_ABOVE_THE_TOP_EDGE{
    .left = 0, .top = -1, .width = 3, .height = 1};

/** @brief An area that starts two columns left of a mask laid at 0, 0 and
 * reaches its first column only. */
constexpr rtype::engine::MaskArea AREA_REACHING_THE_FIRST_COLUMN{
    .left = -2, .top = 0, .width = 3, .height = 1};

/** @brief An area of no column over a mask laid at 0, 0. */
constexpr rtype::engine::MaskArea AREA_OF_NO_COLUMN{
    .left = 0, .top = 0, .width = 0, .height = 2};

/** @brief The cells of a whole WIDE_SIDE row laid at 0, 0. */
constexpr rtype::engine::MaskArea WHOLE_WIDE_ROW{
    .left = 0, .top = 0, .width = WIDE_SIDE, .height = 1};

/** @brief Where a mask is laid away from the origin, and the cell under its
 * top-left pixel there. */
constexpr int MASK_AWAY_LEFT = 10;
constexpr int MASK_AWAY_TOP = 5;
constexpr rtype::engine::MaskArea CELL_UNDER_THE_MOVED_TOP_LEFT{
    .left = MASK_AWAY_LEFT, .top = MASK_AWAY_TOP, .width = 1, .height = 1};
