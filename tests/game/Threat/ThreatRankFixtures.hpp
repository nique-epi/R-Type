#pragma once

#include <cstddef>
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"
#include "ThreatRank.hpp"

namespace rtype::game::testing {

/** @returns The threat of @p rank once it observed @p team for @p ticks
 * ticks, one at a time. */
[[nodiscard]] ThreatLevel levelAfter(ThreatRank& rank,
                                     const TeamFirepower& team,
                                     std::size_t ticks);

}  // namespace rtype::game::testing
