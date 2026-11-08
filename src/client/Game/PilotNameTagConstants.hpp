#pragma once

namespace rtype::client {

/** @brief Pixels between the top of a ship and the line of its name tag. */
constexpr int NAME_TAG_GAP = 2;

/** @brief Pixels of the outline a TextStyle::outlined line adds on each side:
 * one, whatever the scale of the line (see TextStyle::scale). */
constexpr int NAME_TAG_OUTLINE_WIDTH = 1;

}  // namespace rtype::client
