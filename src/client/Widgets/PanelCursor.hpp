#pragma once

namespace rtype::client {

/** @brief Where the next row and the next bar of a tabbed panel go: rows down
 * its panel, bars rightwards under it. */
struct PanelCursor {
  int rowTop{};
  int barLeft{};
};

}  // namespace rtype::client
