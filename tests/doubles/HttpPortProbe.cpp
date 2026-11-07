#include "HttpPortProbe.hpp"
#include <httplib.h>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "MasterHttpServer.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"

namespace {

/** @returns Where a probe listens: @p port of every interface. */
rtype::network::Endpoint everyInterfaceOn(std::uint16_t port) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::ANY_IPV4_ADDRESS}, .port = port};
}

}  // namespace

rtype::master::MasterHttpServer quietHttpServer() {
  return rtype::master::MasterHttpServer{
      []([[maybe_unused]] const std::string& reason) {}};
}

std::uint16_t freeHttpPort() {
  rtype::master::MasterHttpServer probe = quietHttpServer();
  const std::uint16_t port =
      probe.open(everyInterfaceOn(rtype::network::ANY_PORT));
  probe.close();
  return port;
}

bool httpPortIsFree(std::uint16_t port) {
  rtype::master::MasterHttpServer probe = quietHttpServer();
  try {
    static_cast<void>(probe.open(everyInterfaceOn(port)));
  } catch (const rtype::network::HttpPortUnavailableException&) {
    return false;
  }
  probe.close();
  return true;
}

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-unix.BlockInCriticalSection)
httplib::Result httpGet(std::uint16_t port, std::string_view path) {
  httplib::Client client{std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                         port};
  return client.Get(std::string{path});
}

httplib::Result httpSend(std::uint16_t port, const httplib::Request& request) {
  httplib::Client client{std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                         port};
  return client.send(request);
}

std::unique_ptr<httplib::Client> keptAliveClient(std::uint16_t port,
                                                 std::string_view path) {
  auto client = std::make_unique<httplib::Client>(
      std::string{rtype::network::LOOPBACK_IPV4_ADDRESS}, port);
  client->set_keep_alive(true);
  if (!client->Get(std::string{path})) {
    return nullptr;
  }
  return client;
}
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-unix.BlockInCriticalSection)
