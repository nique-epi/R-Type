#include "TeamPercent.hpp"
#include <algorithm>
#include <cstddef>
#include "PercentConstants.hpp"
#include "TeamScaling.hpp"

namespace rtype::game {

int teamPercent(TeamScaling scaling) {
  const auto extraPlayers =
      static_cast<int>(std::max<std::size_t>(scaling.players, 1) - 1);
  return WHOLE_PERCENT + (scaling.percentPerExtraPlayer * extraPlayers);
}

}  // namespace rtype::game
