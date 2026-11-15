#pragma once

#include "InterfaceConstants.hpp"

namespace rtype::client {

/**
 * @brief How large the interface writes its text: how many half pixels of the
 * picture one pixel of a font covers on each axis. NORMAL_TEXT_HALF_PIXELS
 * draws the fonts as they are designed, three draws them half again as large,
 * four twice as large.
 */
struct TextMetrics {
  int halfPixelsPerFontPixel{NORMAL_TEXT_HALF_PIXELS};

  bool operator==(const TextMetrics& other) const = default;
};

}  // namespace rtype::client
