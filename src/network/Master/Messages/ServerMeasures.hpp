#pragma once

#include <optional>

namespace rtype::network {

/**
 * @brief What a server measured of itself, as its heartbeat tells the master.
 * A measure the server does not take is empty, and left out of the JSON: it
 * is never sent as 0.
 */
struct ServerMeasures {
  /** @brief The median and the 95th percentile of the duration of a tick. */
  std::optional<double> tickP50Milliseconds;
  std::optional<double> tickP95Milliseconds;
  /** @brief The bytes of UDP datagrams received and sent each second. */
  std::optional<double> receivedBytesPerSecond;
  std::optional<double> sentBytesPerSecond;
  /** @brief The share of the datagrams of its players lost, 0 to 100. */
  std::optional<double> lossPercent;
  /** @brief The mean round trip to its players. */
  std::optional<double> averagePingMilliseconds;

  bool operator==(const ServerMeasures&) const = default;
};

}  // namespace rtype::network
