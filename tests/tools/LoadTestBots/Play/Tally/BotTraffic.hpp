#pragma once

#include "TrafficTotals.hpp"

namespace rtype::tools {

/** @brief What crossed the socket of a bot, counted twice: as the client reads
 * the datagrams, and as the network carried them. */
struct BotTraffic {
  /** @brief After the decoding of what the server compressed (see
   * client::NetworkThread::traffic()). */
  network::TrafficTotals plain;
  /** @brief Before that decoding (see client::NetworkThread::wireTraffic()). */
  network::TrafficTotals wire;

  bool operator==(const BotTraffic&) const = default;
};

}  // namespace rtype::tools
