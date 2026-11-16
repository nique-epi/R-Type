#pragma once

#include <chrono>

namespace rtype::master {

/** @brief How long the bets of a drawn round are kept, so a player reads what
 * a round paid it after it ended; the dealer removes them past that. */
constexpr std::chrono::hours DRAWN_BETS_KEPT_FOR{24};

}  // namespace rtype::master
