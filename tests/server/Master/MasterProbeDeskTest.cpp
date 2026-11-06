#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <vector>
#include "IncomingDatagram.hpp"
#include "LatestValue.hpp"
#include "MasterProbeDesk.hpp"
#include "MasterProbeDeskFixtures.hpp"
#include "MasterProbeDeskTestConstants.hpp"

using rtype::concurrency::LatestValue;
using rtype::network::IncomingDatagram;
using rtype::server::MasterProbeDesk;
using rtype::server::testing::ANOTHER_SECRET;
using rtype::server::testing::DESK_SECRET;
using rtype::server::testing::FIRST_TOKEN;
using rtype::server::testing::PROBE_ARRIVAL;
using rtype::server::testing::probeArriving;
using rtype::server::testing::SECOND_TOKEN;

/**
 * Given the desk of a server that registered with its secret
 * When a probe with that secret comes
 * Then its token is kept
 */
TEST(MasterProbeDesk, KeepsTheTokenOfAProbeWithItsSecret) {
  LatestValue<std::uint64_t> tokens;
  MasterProbeDesk desk{DESK_SECRET, tokens};

  desk.keep(
      std::vector<IncomingDatagram>{probeArriving(DESK_SECRET, FIRST_TOKEN)},
      PROBE_ARRIVAL);

  EXPECT_EQ(tokens.value(), FIRST_TOKEN);
}

/**
 * Given the same desk
 * When a probe with another secret comes
 * Then nothing is kept
 */
TEST(MasterProbeDesk, IgnoresAProbeWithAnotherSecret) {
  LatestValue<std::uint64_t> tokens;
  MasterProbeDesk desk{DESK_SECRET, tokens};

  desk.keep(
      std::vector<IncomingDatagram>{probeArriving(ANOTHER_SECRET, FIRST_TOKEN)},
      PROBE_ARRIVAL);

  EXPECT_EQ(tokens.value(), std::nullopt);
}

/**
 * Given the same desk, and a token it kept
 * When a forged probe with another secret comes after it
 * Then the token kept is still the first one
 */
TEST(MasterProbeDesk, AForgedProbeNeverReplacesTheToken) {
  LatestValue<std::uint64_t> tokens;
  MasterProbeDesk desk{DESK_SECRET, tokens};

  desk.keep(
      std::vector<IncomingDatagram>{
          probeArriving(DESK_SECRET, FIRST_TOKEN),
          probeArriving(ANOTHER_SECRET, SECOND_TOKEN)},
      PROBE_ARRIVAL);

  EXPECT_EQ(tokens.value(), FIRST_TOKEN);
}

/**
 * Given the same desk
 * When two probes with its secret come in one tick
 * Then the token of the later one stays
 */
TEST(MasterProbeDesk, KeepsTheTokenOfTheLastProbe) {
  LatestValue<std::uint64_t> tokens;
  MasterProbeDesk desk{DESK_SECRET, tokens};

  desk.keep(
      std::vector<IncomingDatagram>{probeArriving(DESK_SECRET, FIRST_TOKEN),
                                    probeArriving(DESK_SECRET, SECOND_TOKEN)},
      PROBE_ARRIVAL);

  EXPECT_EQ(tokens.value(), SECOND_TOKEN);
}
