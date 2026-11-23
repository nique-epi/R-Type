#include "ThreatPercent.hpp"
#include "PercentConstants.hpp"
#include "ThreatScaling.hpp"

namespace rtype::game {

int threatenedPercent(ThreatScaling scaling) {
  return WHOLE_PERCENT +
         (scaling.percentAtTheMost * scaling.threatPercent / WHOLE_PERCENT);
}

}  // namespace rtype::game
