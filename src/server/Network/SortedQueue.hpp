#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief The queue the DatagramSorter puts a datagram of this protocol in once
 * its checks pass. Every queue but Players holds messages sent outside any
 * session, which may carry NO_SESSION_TOKEN and spend the allowance of their
 * address and port among the requests.
 */
enum class SortedQueue : std::uint8_t {
  Players,
  ConnectionRequests,
  InformationRequests,
  Probes,
};

}  // namespace rtype::server
