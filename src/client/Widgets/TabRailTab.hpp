#pragma once

#include <string>

namespace rtype::client {

/** @brief One tab of a TabRail: its label, and whether it is locked, shown
 * but refusing to open until its feature exists. */
struct TabRailTab {
  std::string label;
  bool locked{false};
};

}  // namespace rtype::client
