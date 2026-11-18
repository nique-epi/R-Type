#include "RoulettePhaseDates.hpp"
#include "MillisecondDate.hpp"
#include "RoulettePhase.hpp"

namespace rtype::network {

RoulettePhase roulettePhaseAt(MillisecondDate now, MillisecondDate closesAt,
                              MillisecondDate resultAt) {
  if (now < closesAt) {
    return RoulettePhase::Betting;
  }
  if (now < resultAt) {
    return RoulettePhase::Spinning;
  }
  return RoulettePhase::Result;
}

}  // namespace rtype::network
