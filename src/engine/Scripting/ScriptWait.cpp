#include "ScriptWait.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include "ScriptingConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

std::optional<std::uint32_t> scriptWaitTicks(double milliseconds) {
  if (!std::isfinite(milliseconds) || milliseconds < 0.0) {
    return std::nullopt;
  }
  const double ticks = std::round(
      milliseconds * static_cast<double>(SIMULATION_TICKS_PER_SECOND) /
      SCRIPT_MILLISECONDS_PER_SECOND);
  return static_cast<std::uint32_t>(std::clamp(
      ticks, 1.0, double{std::numeric_limits<std::uint32_t>::max()}));
}

}  // namespace rtype::engine
