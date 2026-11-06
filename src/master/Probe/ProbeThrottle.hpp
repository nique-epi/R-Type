#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include "Endpoint.hpp"

namespace rtype::master {

/**
 * @brief Lets one probe a PROBE_MINIMUM_GAP through to each address and port.
 *
 * It forgets the destinations whose gap passed once it remembers
 * MAXIMUM_THROTTLED_DESTINATIONS of them; past that, only destinations probed
 * within the last gap are kept. Not thread-safe: ProbeSender uses it on
 * ProbeWire only.
 *
 * The RequestAllowances of the game server do not fit: they ban a sender after
 * the drops of a rate limit, so a server that beats faster than its probes
 * would lose them for a ban, and their table of fixed size replaces a
 * destination before its gap passed.
 */
class ProbeThrottle {
 public:
  /**
   * @returns Whether @p destination may get a probe at @p now: none went to it
   * less than PROBE_MINIMUM_GAP before. Notes @p now as its last probe when it
   * may.
   */
  [[nodiscard]] bool allows(const network::Endpoint& destination,
                            std::chrono::steady_clock::time_point now);

 private:
  /** @brief Forgets the destinations whose gap passed at @p now. */
  void forgetPassedGaps(std::chrono::steady_clock::time_point now);

  std::map<std::pair<std::string, std::uint16_t>,
           std::chrono::steady_clock::time_point>
      lastProbeAt_;
};

}  // namespace rtype::master
