#pragma once

#include "OutgoingDatagram.hpp"

namespace rtype::master {

/** @brief Sends the probes of the master to the game servers. */
class IProbeSender {
 public:
  virtual ~IProbeSender() = default;

  /**
   * @brief Sends @p probe on a thread of its own and returns at once, unless
   * its destination got a probe less than PROBE_MINIMUM_GAP ago. A probe it
   * cannot send is written in the journal and dropped. Safe from any thread.
   */
  virtual void sendProbe(network::OutgoingDatagram probe) = 0;
};

}  // namespace rtype::master
