#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "InterfaceColorConstants.hpp"

namespace rtype::client {

/** @brief A ring of equal ping: a dashed line, RING_DASH pixels drawn out of
 * every RING_DASH_PERIOD, and its label RING_LABEL_SHIFT right of where the
 * ring crosses the axis, RING_LABEL_LIFT above the bottom of the map, written
 * only when RING_LABEL_ROOM pixels are left before the right edge. */
inline constexpr float RING_DASH = 2.0F;
inline constexpr float RING_DASH_PERIOD = 5.0F;
inline constexpr float RING_STEP = 1.0F;
inline constexpr int RING_LABEL_SHIFT = 3;
inline constexpr int RING_LABEL_LIFT = 12;
inline constexpr int RING_LABEL_ROOM = 34;
inline constexpr int RING_LABEL_LEFT_MARGIN = 4;
inline constexpr std::string_view PING_UNIT_SUFFIX = " MS";

/** @brief A star is drawn only this far inside the edges of the map, and its
 * glyph grows with its pilots: a dot, a cross, a large cross above
 * CROWDED_SERVER_PILOTS. */
inline constexpr int STAR_MARGIN = 3;
inline constexpr int CROWDED_SERVER_PILOTS = 10;
inline constexpr int EMPTY_STAR_SIZE = 3;
inline constexpr int INCOMPATIBLE_STAR_SIZE = 5;
inline constexpr int STAR_CROSS_REACH = 2;
inline constexpr int CROWDED_STAR_CROSS_REACH = 3;

/** @brief The corners round the chosen star: a box STAR_CORNERS_SIZE wide
 * centred on it, strokes STAR_CORNER_LENGTH long. */
inline constexpr int STAR_CORNERS_SIZE = 15;
inline constexpr int STAR_CORNER_LENGTH = 4;

/** @brief Where the name of a star starts, from the star: LABEL_TEXT_GAP
 * across, LABEL_TEXT_RISE above. */
inline constexpr int LABEL_TEXT_GAP = 7;
inline constexpr int LABEL_TEXT_RISE = 4;

/** @brief The bubble of a server: its title TOOLTIP_PADDING in, its first line
 * TOOLTIP_FIRST_LINE below its top, then a line every TOOLTIP_LINE_PITCH, and
 * TOOLTIP_BOTTOM below the last one; its corners cut TOOLTIP_CUT. */
inline constexpr int TOOLTIP_PADDING = 6;
inline constexpr int TOOLTIP_TITLE_TOP = 5;
inline constexpr int TOOLTIP_FIRST_LINE = 17;
inline constexpr int TOOLTIP_LINE_PITCH = 10;
inline constexpr int TOOLTIP_BOTTOM = 18;

/**
 * @brief The loupe that says the zoom: its picture, a row per string, '#' its
 * rim in cyan, 'l' its glint in white, 'h' its handle in pale blue, the glass
 * inside the rim deep blue; the zoom written LOUPE_TEXT_DROP below its top,
 * LOUPE_TEXT_SHIFT right of it.
 */
inline constexpr std::array<std::string_view, 13> LOUPE_PICTURE{
    "...#####.....", "..#.....#....", ".#.ll....#...", ".#.l.....#...",
    ".#.......#...", ".#.......#...", ".#.......#...", "..#.....#....",
    "...#####hh...", "........hhh..", ".........hhh.", "..........hhh",
    "...........hh"};
inline constexpr char LOUPE_RIM = '#';
inline constexpr char LOUPE_GLINT = 'l';
inline constexpr char LOUPE_HANDLE = 'h';
inline constexpr int LOUPE_GLASS_FIRST_ROW = 1;
inline constexpr int LOUPE_GLASS_LAST_ROW = 7;
inline constexpr int LOUPE_TEXT_SHIFT = 6;
inline constexpr int LOUPE_TEXT_DROP = 16;

/** @brief The minimap: corners cut MINIMAP_CUT, its planet at least
 * MINIMAP_SMALLEST_PLANET pixels in radius, its frame of the part shown at
 * least MINIMAP_SMALLEST_FRAME wide. */
inline constexpr int MINIMAP_SMALLEST_PLANET = 2;
inline constexpr int MINIMAP_SMALLEST_FRAME = 4;

}  // namespace rtype::client
