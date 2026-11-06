#include "MasterProbeDeskFixtures.hpp"
#include <cstdint>
#include <string>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MasterProbe.hpp"
#include "MasterProbeDeskTestConstants.hpp"
#include "PacketHeader.hpp"

namespace rtype::server::testing {

network::IncomingDatagram probeArriving(std::uint64_t secret,
                                        std::uint64_t token) {
  return network::IncomingDatagram{
      .sender = network::Endpoint{.address = std::string{MASTER_ADDRESS},
                                  .port = MASTER_PORT},
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{},
          network::MasterProbe{.secret = secret, .token = token}),
      .receivedAt = PROBE_ARRIVAL};
}

}  // namespace rtype::server::testing
