#include "BydoKind.hpp"
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

}  // namespace rtype::game
