#include "HttpPortProbe.hpp"
#include <cstdint>
#include <string>
#include "HttpConstants.hpp"
#include "HttpEndpoint.hpp"
#include "MasterException.hpp"
#include "MasterHttpServer.hpp"
#include "NetworkConstants.hpp"

namespace {

/** @returns A server of the master that reports nothing when it fails. */
rtype::master::MasterHttpServer quietServer() {
  return rtype::master::MasterHttpServer{
      []([[maybe_unused]] const std::string& reason) {}};
}

/** @returns Where a probe listens: @p port of every interface. */
rtype::master::HttpEndpoint everyInterfaceOn(std::uint16_t port) {
  return rtype::master::HttpEndpoint{
      .address = std::string{rtype::master::ALL_INTERFACES}, .port = port};
}

}  // namespace

std::uint16_t freeHttpPort() {
  rtype::master::MasterHttpServer probe = quietServer();
  const std::uint16_t port =
      probe.open(everyInterfaceOn(rtype::network::ANY_PORT));
  probe.close();
  return port;
}

bool httpPortIsFree(std::uint16_t port) {
  rtype::master::MasterHttpServer probe = quietServer();
  try {
    static_cast<void>(probe.open(everyInterfaceOn(port)));
  } catch (const rtype::master::HttpPortUnavailableException&) {
    return false;
  }
  probe.close();
  return true;
}
