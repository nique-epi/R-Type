#pragma once

#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <future>
#include <optional>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkContext.hpp"
#include "ServerApplication.hpp"
#include "UdpSocket.hpp"

/**
 * @brief A server on the loopback address, and a client socket on the
 * loopback address whose event loop runs on a thread of its own once the test
 * listens on it. Every wait stops after THREAD_TIMEOUT.
 */
class ServerApplicationAfterRunFixture : public ::testing::Test {
 protected:
  ServerApplicationAfterRunFixture();

  /** @brief Stops the event loop of the client, so its thread ends. */
  void TearDown() override;

  rtype::server::ServerApplication& server();

  /**
   * @brief Runs the server on a thread for SERVER_RUN_TICKS ticks, stops it,
   * then waits for run() to return.
   */
  void runThenStop();

  /** @brief Starts receiving on the client, on a thread of its own. */
  void listenOnClient();

  /** @returns The address and port of the client. */
  [[nodiscard]] rtype::network::Endpoint clientEndpoint() const;

  /**
   * @returns The first datagram the client received, or nothing when none
   * arrived within THREAD_TIMEOUT.
   */
  [[nodiscard]] std::optional<std::vector<std::byte>> firstReceivedByClient();

 private:
  rtype::server::ServerApplication server_;
  rtype::network::NetworkContext clientContext_;
  rtype::network::UdpSocket client_;
  std::promise<std::vector<std::byte>> firstDatagram_;
  std::future<std::vector<std::byte>> firstReceived_;
  std::atomic<bool> delivered_{false};
  std::jthread clientThread_;
};
