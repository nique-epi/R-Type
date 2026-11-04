#pragma once

#include <cstddef>
#include "Endpoint.hpp"
#include "RejectionCounts.hpp"

namespace rtype::server {

/**
 * @brief A sender the RejectionLedger rejected datagrams from, with how many
 * for each cause.
 */
struct RejectedSender {
  network::Endpoint endpoint;
  RejectionCounts counts{};
  /**
   * @brief When its latest datagram was rejected, as the number of rejections
   * the ledger recorded until then: the larger, the more recent.
   */
  std::size_t latestRejection{0};
};

}  // namespace rtype::server
