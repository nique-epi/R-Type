#pragma once

#include "TextStyle.hpp"

namespace rtype::client {

/** @brief A column of an ItemList: how far from the left of the list its cells
 * are anchored, on which side of that anchor, and in which font. */
struct ItemListColumn {
  int anchor{};
  TextFont font = TextFont::Regular;
  TextAlignment alignment = TextAlignment::Left;
  /** @brief Above the size the fonts are drawn at, a row takes two lines:
   * the line of the row this column is written on, 0 or 1, and how many
   * letters of the regular font in from the left of the row it is anchored.
   */
  int stackedLine{};
  int stackedLetters{};
};

}  // namespace rtype::client
