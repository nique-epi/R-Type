#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"

/** @brief One datagram the client received, and when. */
struct ReceivedAt {
  std::vector<std::byte> payload;
  std::chrono::steady_clock::time_point time;
};

/** @brief A client of the server on the loopback: a real UDP socket that keeps
 * every datagram it receives, with the time it arrived, and lets a test wait
 * for them. It runs its own thread until it is destroyed. */
class UdpTestClient {
 public:
  UdpTestClient();
  ~UdpTestClient();

  UdpTestClient(const UdpTestClient&) = delete;
  UdpTestClient& operator=(const UdpTestClient&) = delete;
  UdpTestClient(UdpTestClient&&) = delete;
  UdpTestClient& operator=(UdpTestClient&&) = delete;

  /** @brief Sends @p datagram to @p server. */
  void send(const rtype::network::Endpoint& server,
            const std::vector<std::byte>& datagram);

  /**
   * @brief Waits until @p count datagrams arrived in all, at most
   * @p timeout.
   * @returns Every datagram received so far, oldest first.
   */
  [[nodiscard]] std::vector<ReceivedAt> waitForDatagrams(
      std::size_t count, std::chrono::milliseconds timeout);

 private:
  rtype::network::NetworkContext context_;
  rtype::network::UdpSocket socket_;
  std::mutex mutex_;
  std::condition_variable arrived_;
  std::vector<ReceivedAt> received_;
  std::jthread runner_;
};
