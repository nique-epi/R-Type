#include "BydoKind.hpp"
#include <cstdint>
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BydoPointsConstants.hpp"
#include "BydoSettingsConstants.hpp"

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

std::span<const std::int64_t> behaviourSettingsOf(BydoKind kind) {
  switch (kind) {
    case BydoKind::Drifter:
      return DRIFTER_BEHAVIOUR_SETTINGS;
    case BydoKind::Weaver:
      return WEAVER_BEHAVIOUR_SETTINGS;
  }
  return DRIFTER_BEHAVIOUR_SETTINGS;
}

}  // namespace rtype::game
