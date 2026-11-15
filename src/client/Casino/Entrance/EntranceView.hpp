#pragma once

#include <optional>
#include "ButlerLine.hpp"
#include "CasinoBalances.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/** @brief What the casino shows under its bars: the front of the palace, the
 * butler and what he says, and, in the hall only, the panel of its menu and
 * the balances of the player. */
struct EntranceView {
  /** @brief The picture of the front, its doors closed, ajar or open. */
  const char* front{};
  /** @brief The frame of the butler on his sheet, and the column of its left
   * side. */
  game::SpriteFrame butler{};
  int butlerLeft{};
  ButlerLine line;
  /** @brief The balances, shown with the panel of the hall; none before the
   * doors. */
  std::optional<CasinoBalances> hall;
};

}  // namespace rtype::client
