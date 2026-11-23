#pragma once

#include <cstddef>
#include <functional>
#include "EventBus.hpp"
#include "PercentConstants.hpp"

namespace rtype::game {

/** @brief What a BossSystem is built from. */
struct BossSystemParts {
  /** @brief Where the warning and the health of a boss are published, and
   * the destructions heard; it must outlive the system. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief How many players the health of a boss is scaled for, asked as
   * its warning ends. Must not be empty. */
  std::function<std::size_t()> players;
  /** @brief The health of every boss in percent of the health it appeared
   * with, for the difficulty. */
  int healthPercent{WHOLE_PERCENT};
  /** @brief The threat of the team in percent of the most, asked as the
   * warning of a boss ends to raise its health. Must not be empty. */
  std::function<int()> threatPercent;
};

}  // namespace rtype::game
