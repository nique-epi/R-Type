#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramDestinations.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "ServerConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

/** @brief A sorter in front of two queues of the server's sizes. */
class DatagramSorterFixture : public ::testing::Test {
 protected:
  void sort(const rtype::network::Endpoint& sender,
            const std::vector<std::byte>& datagram);

  void admit(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

  void forget(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

  void advanceClock(rtype::engine::Duration elapsed);

  [[nodiscard]] std::size_t droppedCount() const;

  [[nodiscard]] std::size_t wrongTokenCount() const;

  [[nodiscard]] std::optional<rtype::network::Endpoint> takeWrongTokenSender();

  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainPlayers();

  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainRequests();

 private:
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram> players_{
      rtype::server::INCOMING_DATAGRAM_QUEUE_CAPACITY};
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram>
      connectionRequests_{rtype::server::CONNECTION_REQUEST_QUEUE_CAPACITY};
  SimulatedClock clock_;
  rtype::server::DatagramSorter sorter_{
      rtype::server::DatagramDestinations{
          .players = &players_, .connectionRequests = &connectionRequests_},
      clock_};
};
