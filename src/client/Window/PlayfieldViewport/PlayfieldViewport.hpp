#pragma once

namespace rtype::client {

/**
 * @brief Part of the window the playfield is drawn into.
 *
 * Every member is a fraction of the window size, between 0 and 1: left and
 * width along the horizontal axis, top and height along the vertical axis.
 * What lies outside the rectangle is left to the black bars.
 */
struct PlayfieldViewport {
  float left;
  float top;
  float width;
  float height;
};

/**
 * @brief Finds the largest centered rectangle of the window that has the
 * proportions of the playfield.
 *
 * A window wider than the playfield gets equal bars on its left and right, a
 * taller one gets equal bars above and below, and a window with the same
 * proportions is covered entirely.
 *
 * @param windowWidth  Width of the window, in pixels.
 * @param windowHeight Height of the window, in pixels.
 * @returns The rectangle. It covers the whole window when the window has no
 * area.
 */
PlayfieldViewport fitPlayfieldInWindow(unsigned int windowWidth,
                                       unsigned int windowHeight);

}  // namespace rtype::client
