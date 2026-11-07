#pragma once

#include "BossBarLook.hpp"
#include "EdgePulse.hpp"
#include "WarningBanner.hpp"
#include "WatchOut.hpp"

namespace rtype::client {

/** @brief What the boss lays over the playfield: the WARNING screen, the
 * tint of the edges, its bar, and the warning of its charge. */
struct BossLook {
  WarningBannerLook warning;
  EdgePulseLook edges;
  BossBarLook bar;
  WatchOutLook watchOut;

  bool operator==(const BossLook&) const = default;
};

}  // namespace rtype::client
