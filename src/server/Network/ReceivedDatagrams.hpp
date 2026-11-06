#pragma once

#include <span>
#include "IncomingDatagram.hpp"

namespace rtype::server {

/**
 * @brief What the ServerApplication received since the previous tick, oldest
 * first in each list; often nothing.
 */
struct ReceivedDatagrams {
  /** @brief The datagrams of the players admitted with
   * ServerApplication::admit(), each carrying the session token of its
   * player. */
  std::span<const network::IncomingDatagram> players;
  /** @brief The datagrams holding a ConnectionRequest, from strangers or from
   * players with their token or none: at most
   * CONNECTION_REQUEST_QUEUE_CAPACITY. */
  std::span<const network::IncomingDatagram> connectionRequests;
  /** @brief The datagrams holding a ServerInformationRequest and no
   * ConnectionRequest, from anyone: at most
   * INFORMATION_REQUEST_QUEUE_CAPACITY. */
  std::span<const network::IncomingDatagram> informationRequests;
  /** @brief The datagrams holding a MasterProbe and no request, from anyone:
   * at most PROBE_QUEUE_CAPACITY. */
  std::span<const network::IncomingDatagram> probes;
};

}  // namespace rtype::server
