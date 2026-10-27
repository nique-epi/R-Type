#pragma once

#include "PlayerColors.hpp"

namespace rtype::client {

/** @brief The three shades of a player color a copy of a sheet frame shows
 * (from), and the three it paints in their place (to): those the palette the
 * player chose gives the same player. */
struct ShadeSwap {
  PlayerColors from;
  PlayerColors to;

  bool operator==(const ShadeSwap& other) const = default;
};

}  // namespace rtype::client
