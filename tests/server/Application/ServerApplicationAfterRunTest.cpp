#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <vector>
#include "OutgoingDatagram.hpp"
#include "ServerApplicationAfterRunFixture.hpp"
#include "ServerTestConstants.hpp"

using rtype::network::OutgoingDatagram;

/**
 * Given a server whose run() returned once it was stopped, and a client
 * listening on the loopback address
 * When a datagram to the client is queued, then sendQueuedAfterRun() is
 * called
 * Then the client receives that datagram
 */
TEST_F(ServerApplicationAfterRunFixture, SendsWhatWasQueuedOnceRunReturned) {
  runThenStop();
  listenOnClient();
  const std::vector<std::byte> payload(CLIENT_PAYLOAD_SIZE,
                                       CLIENT_PAYLOAD_BYTE);

  server().queueDatagram(
      OutgoingDatagram{.destination = clientEndpoint(), .payload = payload});
  server().sendQueuedAfterRun();

  EXPECT_EQ(firstReceivedByClient(),
            std::optional<std::vector<std::byte>>{payload});
}
