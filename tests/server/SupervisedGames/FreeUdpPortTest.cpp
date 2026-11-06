#include <gtest/gtest.h>
#include <cstdint>
#include "FreeUdpPort.hpp"
#include "UdpPortProbe.hpp"

/**
 * Given nothing bound
 * When a free port is asked for
 * Then it is a real port, and a socket can be bound to it right after
 */
TEST(FreeUdpPort, GivesAPortThatCanBeBoundRightAfter) {
  const std::uint16_t port = rtype::server::freeUdpPort();

  EXPECT_NE(port, 0);
  EXPECT_TRUE(udpPortIsFree(port));
}
