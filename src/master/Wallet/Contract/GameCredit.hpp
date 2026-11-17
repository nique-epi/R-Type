#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace rtype::master {

/** @brief What a finished game pays the account of one of its players. */
struct GameCredit {
  /** @brief The game ticket the player joined with, as a game server reports
   * it: it names the account to pay, which only the master knows. */
  std::string ticket;
  /** @brief The kopecks it pays (network::kopecksFor() and
   * network::withVictoryBonus()); the balance stops at
   * network::MAXIMUM_KOPECKS. */
  std::uint32_t kopecks{};
  /** @brief The looks the player won in the game, by their names in
   * identifiers::SHIP_LOOK_ITEM_NAMES; a name of no look is skipped. */
  std::vector<std::string> wonLooks;
  /** @brief The result it pays, such as `game:<server>:<game>:<nickname>`:
   * a credit of a reference already credited applies nothing. */
  std::string reference;
  std::chrono::sys_seconds creditedAt;
};

}  // namespace rtype::master
