#pragma once

#include "TextStyle.hpp"

namespace rtype::client {

/** @brief A column of an ItemList: how far from the left of the list its cells
 * are anchored, on which side of that anchor, and in which font. */
struct ItemListColumn {
  int anchor{};
  TextFont font = TextFont::Regular;
  TextAlignment alignment = TextAlignment::Left;
};

}  // namespace rtype::client
