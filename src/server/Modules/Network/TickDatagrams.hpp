#pragma once

#include <span>
#include "IncomingDatagram.hpp"
#include "ServerApplication.hpp"

namespace rtype::server {

/**
 * @brief The datagrams the current tick works on, handed from the network
 * module, which receives them, to the modules whose work reads them. A work
 * of a stage only receives the time of the tick, so the datagrams come through
 * this service.
 *
 * Between two ticks it holds nothing. The spans it hands out stay valid until
 * the tick ends.
 */
class TickDatagrams {
 public:
  /** @brief Keeps @p received for the tick that starts. The datagrams must
   * outlive the tick. */
  void begin(const ServerApplication::ReceivedDatagrams& received);

  /** @brief Forgets the datagrams of the tick that ended. */
  void end();

  /** @returns The datagrams of the players admitted, oldest first. */
  [[nodiscard]] std::span<const network::IncomingDatagram> players() const;

  /** @returns The datagrams holding a connection request, oldest first. */
  [[nodiscard]] std::span<const network::IncomingDatagram> connectionRequests()
      const;

 private:
  ServerApplication::ReceivedDatagrams received_{};
};

}  // namespace rtype::server
