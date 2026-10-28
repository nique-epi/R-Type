#include "ServerModuleFixture.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "Endpoint.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"

ServerModuleFixture::ServerModuleFixture()
    : journalPath_(journal_.path().string()),
      arguments_{rtype::logging::FILE_OPTION, journalPath_,
                 rtype::logging::NO_STDERR_OPTION} {}

rtype::logging::LogLaunchOptions ServerModuleFixture::journalOptions() const {
  return rtype::logging::LogLaunchOptions::read(
      arguments_, rtype::logging::SERVER_LOG_FILE_NAME);
}

std::uint16_t ServerModuleFixture::freeUdpPort() {
  rtype::network::NetworkContext network;
  const rtype::network::UdpSocket socket{
      network, rtype::network::Endpoint{
                   .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
                   .port = rtype::network::ANY_PORT}};
  return socket.localEndpoint().port;
}
