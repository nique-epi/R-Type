#include "MasterProbeModuleFixture.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "CreatedServer.hpp"
#include "DatagramCodec.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterProbe.hpp"
#include "MasterProbeModuleTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerRegistration.hpp"
#include "UdpTestClient.hpp"

MasterProbeModuleFixture::MasterProbeModuleFixture()
    : created_(master_.createServer(TEST_SERVER_NAME)),
      server_(rtype::network::MasterAccess{.url = master_.url(),
                                           .key = created_.key}) {}

void MasterProbeModuleFixture::registerClient() {
  rtype::network::ServerRegistration registration =
      rtype::network::testing::parisRegistration();
  registration.address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS};
  registration.udpPort = client_.localEndpoint().port;
  server_.registerServer(registration);
}

void MasterProbeModuleFixture::sendHeartbeat() {
  server_.sendHeartbeat(rtype::network::testing::parisHeartbeat());
}

std::vector<rtype::network::MasterProbe> MasterProbeModuleFixture::probesSoon(
    std::size_t count) {
  std::vector<rtype::network::MasterProbe> probes;
  for (const ReceivedAt& received :
       client_.waitForDatagrams(count, PROBE_DEADLINE)) {
    probes.push_back(rtype::network::DatagramCodec::firstMessage<
                         rtype::network::MasterProbe>(received.payload)
                         .value_or(rtype::network::MasterProbe{}));
  }
  return probes;
}
