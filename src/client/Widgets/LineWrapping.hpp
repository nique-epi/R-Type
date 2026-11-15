#pragma once

#include "TextStyle.hpp"

namespace rtype::client {

/** @brief How a text is cut into lines: how wide a line may run, in pixels of
 * the interface picture, and the font it is measured in. */
struct LineWrapping {
  int width{};
  TextFont font = TextFont::Regular;
};

}  // namespace rtype::client
