#pragma once

namespace rtype::client::testing {

/** @brief The book lies in the middle of the 480 x 270 picture: closed, its
 * 208-pixel cover from column 136; open, its hinge on column 240; its height
 * is 206 rows. */
constexpr int CLOSED_LEFT = 136;
constexpr int MIDDLE_COLUMN = 240;
constexpr int PICTURE_WIDTH = 480;
constexpr int PICTURE_HEIGHT = 270;
constexpr int BOOK_HEIGHT = 206;

}  // namespace rtype::client::testing
