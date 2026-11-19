#pragma once

#include <optional>
#include <string>
#include "BossGaugeLook.hpp"

namespace rtype::client {

/** @brief The whole bar: whether it shows, the name of the boss written above
 * it (none when its sheet gives none), its two gauges, which set of hot
 * pixels the rage shows (0 outside the rage), and how many milliseconds into
 * its shatter gauge I is, or nothing. */
struct BossBarLook {
  bool shown{};
  std::string name;
  BossGaugeLook phaseOne{};
  BossGaugeLook phaseTwo{};
  int flickerStep{};
  std::optional<int> shatterMilliseconds;

  bool operator==(const BossBarLook&) const = default;
};

}  // namespace rtype::client
