#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"

namespace rtype::network {

class DelayedDelivery;

/**
 * @brief A socket whose datagrams cross a simulated network: it hands what it
 * sends to the real socket, and what the real socket receives to its own
 * handler, each after the delays, losses, copies and reordering the
 * NetworkConditions describe (see DatagramsInFlight).
 *
 * Like the real socket, it runs on the event loop of a NetworkContext: use it,
 * and destroy it, on the thread that runs that context, or before that thread
 * starts. Destroying it drops what is still in flight.
 */
class SimulatedNetworkSocket final : public IDatagramSocket {
 public:
  /** @param socket The real socket the datagrams go through; it is owned. */
  SimulatedNetworkSocket(NetworkContext& context,
                         std::unique_ptr<IDatagramSocket> socket,
                         const NetworkConditions& conditions);

  /** @brief Drops what is in flight, then closes the real socket. */
  ~SimulatedNetworkSocket() override;

  SimulatedNetworkSocket(const SimulatedNetworkSocket&) = delete;
  SimulatedNetworkSocket& operator=(const SimulatedNetworkSocket&) = delete;
  SimulatedNetworkSocket(SimulatedNetworkSocket&&) = delete;
  SimulatedNetworkSocket& operator=(SimulatedNetworkSocket&&) = delete;

  /**
   * @brief Hands every datagram the real socket receives from now on to
   * @p handler, once it crossed the simulated network.
   * @throws ReceivingAlreadyStartedException as the real socket does.
   */
  void startReceiving(DatagramHandler handler) override;

  /**
   * @brief Copies @p payload and hands it to the real socket once it crossed
   * the simulated network, then returns at once.
   * @throws DatagramTooLargeException or InvalidAddressException at once,
   * as the real socket would: nothing is sent.
   */
  void send(const Endpoint& destination,
            std::span<const std::byte> payload) override;

  /** @returns The address and port of the real socket. */
  [[nodiscard]] Endpoint localEndpoint() const override;

 private:
  NetworkContext* context_;
  NetworkConditions conditions_;
  std::unique_ptr<IDatagramSocket> socket_;
  std::shared_ptr<DelayedDelivery> sends_;
  std::shared_ptr<DelayedDelivery> receives_;
};

}  // namespace rtype::network
