#include <gtest/gtest.h>
#include <vector>
#include "MasterMessageTextTestConstants.hpp"
#include "MasterProbe.hpp"
#include "MasterProbeModuleFixture.hpp"
#include "MasterProbeModuleTestConstants.hpp"

using rtype::network::MasterProbe;
using rtype::network::testing::PARIS_PROBE_SECRET;

/**
 * Given a server registered at the port of a UDP client on the loopback
 * When it sends a heartbeat
 * Then the client receives a probe of the master, with the secret the server
 * registered with
 */
TEST_F(MasterProbeModuleFixture, AHeartbeatMakesTheMasterSendAProbe) {
  registerClient();

  sendHeartbeat();

  const std::vector<MasterProbe> probes = probesSoon(ONE_PROBE);
  ASSERT_EQ(probes.size(), ONE_PROBE);
  EXPECT_EQ(probes.front().secret, PARIS_PROBE_SECRET);
}

/**
 * Given the same server
 * When it sends two heartbeats at once
 * Then the client receives one probe: a destination gets one a second at
 * most
 */
TEST_F(MasterProbeModuleFixture, TwoHeartbeatsAtOnceMakeOneProbe) {
  registerClient();

  sendHeartbeat();
  sendHeartbeat();

  EXPECT_EQ(probesSoon(TWO_PROBES).size(), ONE_PROBE);
}
