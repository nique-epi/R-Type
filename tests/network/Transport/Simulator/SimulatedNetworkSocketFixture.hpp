#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConditions.hpp"
#include "NetworkContext.hpp"
#include "RecordingDatagramSocket.hpp"
#include "SimulatedNetworkSocket.hpp"

/**
 * @brief The event loop the simulated socket of a test runs on, and the
 * recording socket that stands for the real one behind it.
 */
class SimulatedNetworkSocketFixture : public ::testing::Test {
 protected:
  /** @returns A simulated socket over a RecordingDatagramSocket, which
   * recording() reads while the simulated socket lives. */
  [[nodiscard]] std::unique_ptr<rtype::network::SimulatedNetworkSocket>
  overRecording(const rtype::network::NetworkConditions& conditions);

  [[nodiscard]] RecordingDatagramSocket& recording();

  [[nodiscard]] rtype::network::NetworkContext& context();

  /** @brief Runs the event loop until one handler ran, for DATAGRAM_TIMEOUT
   * at most. @returns Whether one ran. */
  [[nodiscard]] bool runOneHandler();

  /** @brief Runs the event loop for @p duration at most, or until one handler
   * ran. */
  void runAtMostOneHandlerFor(std::chrono::milliseconds duration);

  /** @brief Runs the event loop until it has no work left. */
  void runUntilIdle();

  /** @brief Runs the event loop until the recording socket was handed
   * @p count datagrams, for DATAGRAM_TIMEOUT at most.
   * @returns Whether it was. */
  [[nodiscard]] bool runUntilSent(std::size_t count);

  /** @returns A handler that appends every datagram it gets to @p received. */
  [[nodiscard]] static rtype::network::DatagramHandler recordInto(
      std::vector<rtype::network::IncomingDatagram>& received);

 private:
  rtype::network::NetworkContext context_;
  RecordingDatagramSocket* recording_{nullptr};
};
