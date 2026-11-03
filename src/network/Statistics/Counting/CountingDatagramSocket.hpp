#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "TrafficCounters.hpp"

namespace rtype::network {

/**
 * @brief A socket that counts what goes through another socket, which it owns.
 *
 * Put it over the network simulator, which plays the part of the network: a
 * datagram lost on the way out was sent, and one lost on the way in was never
 * received, as on a real network.
 *
 * Like any socket, use it on the thread that runs its NetworkContext.
 */
class CountingDatagramSocket final : public IDatagramSocket {
 public:
  /** @param counters Must outlive this socket and every handler it keeps. */
  CountingDatagramSocket(std::unique_ptr<IDatagramSocket> socket,
                         TrafficCounters& counters);

  /** @brief Counts each datagram received, then hands it to @p handler.
   * @throws What the wrapped socket throws. */
  void startReceiving(DatagramHandler handler) override;

  /** @brief Hands @p payload to the wrapped socket, then counts it.
   * @throws What the wrapped socket throws: nothing is counted then. */
  void send(const Endpoint& destination,
            std::span<const std::byte> payload) override;

  /** @returns The address and port of the wrapped socket. */
  [[nodiscard]] Endpoint localEndpoint() const override;

 private:
  std::unique_ptr<IDatagramSocket> socket_;
  TrafficCounters* counters_;
};

}  // namespace rtype::network
