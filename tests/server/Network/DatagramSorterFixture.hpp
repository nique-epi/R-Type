#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "RejectionLedger.hpp"
#include "RejectionReport.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

/**
 * @brief A sorter in front of two queues of the server's sizes, counting its
 * rejections in a ledger the test reads.
 */
class DatagramSorterFixture : public ::testing::Test {
 protected:
  DatagramSorterFixture();

  void sort(const rtype::network::Endpoint& sender,
            const std::vector<std::byte>& datagram);
  /** @brief Sorts @p count copies of @p datagram from @p sender, all at
   * once. */
  void sortRepeatedly(const rtype::network::Endpoint& sender,
                      const std::vector<std::byte>& datagram,
                      std::size_t count);
  void admit(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
  void forget(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
  void advanceClock(rtype::engine::Duration elapsed);
  [[nodiscard]] rtype::server::RejectionReport takeReport();
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainPlayers();
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainRequests();

 private:
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram> players_;
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram>
      connectionRequests_;
  SimulatedClock clock_;
  rtype::server::RejectionLedger rejections_;
  rtype::server::DatagramSorter sorter_;
};
