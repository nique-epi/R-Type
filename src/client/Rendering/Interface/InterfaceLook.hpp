#pragma once

#include "TextMetrics.hpp"

namespace rtype::client {

/** @brief What the player chose for the look of every interface picture: how
 * large its text is, and whether it is drawn in high contrast. */
struct InterfaceLook {
  TextMetrics text;
  bool highContrast = false;

  bool operator==(const InterfaceLook& other) const = default;
};

}  // namespace rtype::client
