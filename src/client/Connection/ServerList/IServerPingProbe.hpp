#pragma once

#include <vector>
#include "Endpoint.hpp"
#include "PingAnswer.hpp"

namespace rtype::client {

/**
 * @brief Measures the round trip to game servers with a
 * ServerInformationRequest each, the way the list of servers needs it: a round
 * of requests at once, then the answers as they arrive. Neither call makes the
 * frame wait.
 */
class IServerPingProbe {
 public:
  virtual ~IServerPingProbe() = default;

  /**
   * @brief Starts a new round: one request to each of @p servers, in place of
   * the round under way, whose answers still on their way are never read.
   */
  virtual void measure(std::vector<network::Endpoint> servers) = 0;

  /** @returns The answers of the round that arrived since the previous call,
   * at most one per server, the first it sent. */
  [[nodiscard]] virtual std::vector<PingAnswer> answers() = 0;
};

}  // namespace rtype::client
