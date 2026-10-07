#pragma once

#include <gtest/gtest.h>
#include <asio/ip/udp.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkContext.hpp"

/**
 * @brief The event loop a UdpSocket test runs on, and what the tests share:
 *        payloads, a recorder for the datagrams a socket hands over, and a
 *        plain Asio client on the loopback address.
 *
 * The client shares no code with UdpSocket, so a socket is never checked
 * against itself. Every wait stops after DATAGRAM_TIMEOUT: a failing test
 * fails instead of hanging.
 */
class UdpSocketFixture : public ::testing::Test {
 protected:
  /**
   * @brief A datagram as a socket handed it to its handler.
   */
  struct ReceivedDatagram {
    rtype::network::Endpoint sender;
    std::vector<std::byte> payload;
  };

  rtype::network::NetworkContext& context();

  static rtype::network::Endpoint loopbackOnPort(std::uint16_t port);

  /**
   * @return @p size bytes, each one different from the next, so that a lost,
   *         shifted or swapped byte shows.
   */
  static std::vector<std::byte> makePayload(std::size_t size);

  /**
   * @return a handler that appends every datagram it gets to @p received.
   */
  static rtype::network::DatagramHandler recordInto(
      std::vector<ReceivedDatagram>& received);

  /**
   * @brief Runs the event loop until @p isDone holds, for DATAGRAM_TIMEOUT at
   *        most.
   * @return whether @p isDone holds.
   */
  bool runUntil(const std::function<bool()>& isDone);

  /**
   * @brief Runs the event loop until it has no work left, for
   *        DATAGRAM_TIMEOUT at most.
   */
  void runUntilIdle();

  /**
   * @brief Opens a UdpSocket on @p localEndpoint, then destroys it.
   */
  void openSocketOn(const rtype::network::Endpoint& localEndpoint);

  /**
   * @return a plain Asio socket on the loopback address, on a port the system
   *         picked.
   */
  asio::ip::udp::socket openClient();

  /**
   * @brief Sends @p payload from @p client to @p port on the loopback address,
   *        before returning.
   */
  static void sendFromClient(asio::ip::udp::socket& client, std::uint16_t port,
                             std::span<const std::byte> payload);

  /**
   * @return the next datagram @p client receives, or nothing after
   *         DATAGRAM_TIMEOUT.
   */
  std::optional<std::vector<std::byte>> receiveOnClient(
      asio::ip::udp::socket& client);

 private:
  rtype::network::NetworkContext context_;
};
