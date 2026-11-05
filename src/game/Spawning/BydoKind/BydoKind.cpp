#include "BydoKind.hpp"
#include <cstdint>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BydoPointsConstants.hpp"

namespace rtype::game {

std::string_view behaviourNameOf(BydoKind kind) {
  switch (kind) {
    case BydoKind::Drifter:
      return STRAIGHT_BEHAVIOUR_NAME;
    case BydoKind::Weaver:
      return WEAVE_BEHAVIOUR_NAME;
  }
  return STRAIGHT_BEHAVIOUR_NAME;
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
