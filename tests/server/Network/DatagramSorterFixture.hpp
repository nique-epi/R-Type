#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BoundedQueue.hpp"
#include "DatagramDrops.hpp"
#include "DatagramSorter.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

/** @brief A sorter in front of three queues of the server's sizes; the
 * information requests are followed by DatagramSorterInformationFixture. */
class DatagramSorterFixture : public ::testing::Test {
 protected:
  DatagramSorterFixture();

  void sort(const rtype::network::Endpoint& sender,
            const std::vector<std::byte>& datagram);
  void admit(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
  void forget(
      const rtype::network::Endpoint& endpoint,
      std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
  /** @brief Sorts @p count copies of @p datagram from @p sender, all at
   * once. */
  void sortRepeatedly(const rtype::network::Endpoint& sender,
                      const std::vector<std::byte>& datagram,
                      std::size_t count);
  void advanceClock(rtype::engine::Duration elapsed);
  [[nodiscard]] std::size_t droppedCount() const;
  [[nodiscard]] std::size_t wrongTokenCount() const;
  [[nodiscard]] std::optional<rtype::network::Endpoint> takeWrongTokenSender();
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainPlayers();
  [[nodiscard]] std::vector<rtype::network::IncomingDatagram> drainRequests();

 private:
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram> players_;
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram>
      connectionRequests_;
  rtype::concurrency::BoundedQueue<rtype::network::IncomingDatagram>
      informationRequests_;
  SimulatedClock clock_;
  rtype::server::DatagramDrops drops_;
  rtype::server::DatagramSorter sorter_;
};
