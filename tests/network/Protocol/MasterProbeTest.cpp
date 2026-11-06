#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <vector>
#include "DatagramCodec.hpp"
#include "MasterProbe.hpp"
#include "MasterProbeTestConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"

using rtype::network::BufferUnderflowException;
using rtype::network::DatagramCodec;
using rtype::network::decodePayload;
using rtype::network::MasterProbe;
using rtype::network::PacketHeader;
using rtype::network::TrailingBytesException;
using rtype::network::testing::PROBE_DATAGRAM_BYTES;
using rtype::network::testing::PROBE_PAYLOAD_ONE_LONG;
using rtype::network::testing::PROBE_PAYLOAD_ONE_SHORT;
using rtype::network::testing::PROBE_SECRET;
using rtype::network::testing::PROBE_TOKEN;

/**
 * Given a probe with a secret and a token
 * When it is encoded in a datagram, then read back
 * Then the same probe is read
 */
TEST(MasterProbe, ReadsBackAWrittenProbe) {
  const MasterProbe probe{.secret = PROBE_SECRET, .token = PROBE_TOKEN};

  const std::optional<MasterProbe> read =
      DatagramCodec::firstMessage<MasterProbe>(
          DatagramCodec::encode(PacketHeader{}, probe));

  EXPECT_EQ(read, probe);
}

/**
 * Given a probe
 * When it is encoded in a datagram
 * Then the datagram is 41 bytes long, as the protocol page says
 */
TEST(MasterProbe, MakesADatagramOfFortyOneBytes) {
  const MasterProbe probe{.secret = PROBE_SECRET, .token = PROBE_TOKEN};

  EXPECT_EQ(DatagramCodec::encode(PacketHeader{}, probe).size(),
            PROBE_DATAGRAM_BYTES);
}

/**
 * Given a payload one byte short of a probe
 * When it is read
 * Then it is refused
 */
TEST(MasterProbe, RefusesAPayloadCutShort) {
  const std::vector<std::byte> payload(PROBE_PAYLOAD_ONE_SHORT);

  EXPECT_THROW(static_cast<void>(
                   decodePayload(payload, std::type_identity<MasterProbe>{})),
               BufferUnderflowException);
}

/**
 * Given a payload one byte longer than a probe
 * When it is read
 * Then it is refused
 */
TEST(MasterProbe, RefusesABytePastTheToken) {
  const std::vector<std::byte> payload(PROBE_PAYLOAD_ONE_LONG);

  EXPECT_THROW(static_cast<void>(
                   decodePayload(payload, std::type_identity<MasterProbe>{})),
               TrailingBytesException);
}
