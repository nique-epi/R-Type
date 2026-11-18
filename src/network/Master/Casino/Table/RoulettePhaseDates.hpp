#pragma once

#include "MillisecondDate.hpp"
#include "RoulettePhase.hpp"

namespace rtype::network {

/** @returns The phase of a round at @p now, read on its dates: the bets open
 * until @p closesAt excluded, the wheel spinning until @p resultAt excluded,
 * the result from then on. */
[[nodiscard]] RoulettePhase roulettePhaseAt(MillisecondDate now,
                                            MillisecondDate closesAt,
                                            MillisecondDate resultAt);

}  // namespace rtype::network
