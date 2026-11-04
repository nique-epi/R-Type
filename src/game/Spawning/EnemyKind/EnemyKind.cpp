#include "EnemyKind.hpp"
#include <cstdint>
#include "EnemyPointsConstants.hpp"
#include "MovementPattern.hpp"

namespace rtype::game {

PatternShape patternShapeOf(EnemyKind kind) {
  switch (kind) {
    case EnemyKind::Weaver:
      return PatternShape::Wave;
    case EnemyKind::Drifter:
    case EnemyKind::Voltjelly:
    case EnemyKind::Reaper:
    case EnemyKind::Bydomancer:
    case EnemyKind::Drillback:
    case EnemyKind::BydoTyrant:
      return PatternShape::Straight;
  }
  return PatternShape::Straight;
}

std::uint32_t pointsOf(EnemyKind kind) {
  switch (kind) {
    case EnemyKind::Weaver:
      return WEAVER_POINTS;
    case EnemyKind::Drifter:
    case EnemyKind::Voltjelly:
    case EnemyKind::Reaper:
    case EnemyKind::Bydomancer:
    case EnemyKind::Drillback:
    case EnemyKind::BydoTyrant:
      return DRIFTER_POINTS;
  }
  return 0;
}

}  // namespace rtype::game
