#include "BydoKind.hpp"
#include <cstdint>
#include "BydoPointsConstants.hpp"
#include "MovementPattern.hpp"

namespace rtype::game {

PatternShape patternShapeOf(BydoKind kind) {
  switch (kind) {
    case BydoKind::Drifter:
      return PatternShape::Straight;
    case BydoKind::Weaver:
      return PatternShape::Wave;
  }
  return PatternShape::Straight;
}

std::uint32_t pointsOf(BydoKind kind) {
  switch (kind) {
    case BydoKind::Drifter:
      return DRIFTER_POINTS;
    case BydoKind::Weaver:
      return WEAVER_POINTS;
  }
  return 0;
}

}  // namespace rtype::game
