#pragma once

#include "BossBarLook.hpp"
#include "EdgePulse.hpp"
#include "WarningBanner.hpp"

namespace rtype::client {

/** @brief What the boss lays over the playfield: the WARNING screen, the
 * tint of the edges, and its bar. */
struct BossLook {
  WarningBannerLook warning;
  EdgePulseLook edges;
  BossBarLook bar;

  bool operator==(const BossLook&) const = default;
};

}  // namespace rtype::client
