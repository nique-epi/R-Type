#pragma once

#include <cstdint>
#include <optional>
#include <thread>
#include "IProbeSender.hpp"
#include "NetworkContext.hpp"
#include "OutgoingDatagram.hpp"
#include "ProbeThrottle.hpp"
#include "ThrottledLog.hpp"
#include "UdpSocket.hpp"

namespace rtype::master {

/**
 * @brief Sends the probes of the master from a UDP socket that never
 * receives, on ProbeWire, a thread of its own.
 *
 * open() binds the socket to a port the system picks and starts ProbeWire,
 * which runs its NetworkContext until close(); sendProbe() hands it a probe
 * through NetworkContext::post(). Every send is caught on ProbeWire, so
 * nothing leaves the thread. The socket is opened before ProbeWire starts and
 * closed after it stops, as a socket asks.
 */
class ProbeSender final : public IProbeSender {
 public:
  ProbeSender();
  ~ProbeSender() override;

  ProbeSender(const ProbeSender&) = delete;
  ProbeSender& operator=(const ProbeSender&) = delete;
  ProbeSender(ProbeSender&&) = delete;
  ProbeSender& operator=(ProbeSender&&) = delete;

  /**
   * @brief Binds the socket on every interface to a port the system picks,
   * then starts ProbeWire; closes the socket again when the thread cannot
   * start. Called once.
   * @returns The port the probes leave from.
   * @throws network::SocketOpenException when the socket cannot be opened.
   */
  std::uint16_t open();

  /** @brief Stops ProbeWire, once the probe it sends is sent, then closes the
   * socket; the probes still queued are dropped. Does nothing when it is not
   * open. */
  void close();

  void sendProbe(network::OutgoingDatagram probe) override;

 private:
  /** @brief Sends @p probe now, on ProbeWire, unless ProbeThrottle holds it
   * back; a probe the socket refuses is logged. */
  void sendNow(const network::OutgoingDatagram& probe);

  network::NetworkContext network_;
  std::optional<network::UdpSocket> socket_;
  ProbeThrottle throttle_;
  logging::ThrottledLog failureLog_;
  std::jthread probeWire_;
};

}  // namespace rtype::master
