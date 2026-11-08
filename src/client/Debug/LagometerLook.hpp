#pragma once

namespace rtype::client {

/** @brief How the player set the lagometer to look: how many times larger
 * than its smallest, and the opacity of its panel, in percent. */
struct LagometerLook {
  int scale{1};
  int fillPercent{0};

  bool operator==(const LagometerLook&) const = default;
};

}  // namespace rtype::client
