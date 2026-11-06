#include "ProbeThrottle.hpp"
#include <chrono>
#include <map>
#include "Endpoint.hpp"
#include "ProbeConstants.hpp"

namespace rtype::master {

bool ProbeThrottle::allows(const network::Endpoint& destination,
                           std::chrono::steady_clock::time_point now) {
  if (lastProbeAt_.size() >= MAXIMUM_THROTTLED_DESTINATIONS) {
    forgetPassedGaps(now);
  }
  const auto [entry, added] =
      lastProbeAt_.try_emplace({destination.address, destination.port}, now);
  if (added) {
    return true;
  }
  if (now - entry->second < PROBE_MINIMUM_GAP) {
    return false;
  }
  entry->second = now;
  return true;
}

void ProbeThrottle::forgetPassedGaps(
    std::chrono::steady_clock::time_point now) {
  std::erase_if(lastProbeAt_, [now](const auto& entry) {
    return now - entry.second >= PROBE_MINIMUM_GAP;
  });
}

}  // namespace rtype::master
