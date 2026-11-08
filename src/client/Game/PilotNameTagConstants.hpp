#pragma once

namespace rtype::client {

/** @brief Pixels between the top of a ship and the line of its name tag. */
constexpr int NAME_TAG_GAP = 2;

/** @brief Pixels of the outline a TextStyle::outlined line adds on each side:
 * one, whatever the scale of the line (see TextStyle::scale). */
constexpr int NAME_TAG_OUTLINE_WIDTH = 1;

/**
 * @brief The most pixels a byte of a name tag takes, written in the small
 * font: a printable ASCII character of Silkscreen is at most 7 pixels wide,
 * and its widest character, 10 pixels (advanceWidthMax 1250 for 1000 units
 * per em, drawn at 8), lies outside ASCII and takes at least 2 bytes of UTF-8.
 * So a line is never wider than this many pixels per byte of its text.
 */
constexpr int NAME_TAG_PIXELS_PER_BYTE = 8;

}  // namespace rtype::client
