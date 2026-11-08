#include "MasterHttpServer.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include "ApiHandler.hpp"
#include "Endpoint.hpp"
#include "FailureReport.hpp"
#include "ForwardedAddress.hpp"
#include "MasterConstants.hpp"
#include "MasterRoute.hpp"
#include "MasterRoutePath.hpp"

namespace rtype::master {

MasterHttpServer::MasterHttpServer(FailureReport onFailure)
    : server_(std::string{MASTER_LOGGER_NAME}, std::move(onFailure),
              network::ForwardedAddress::TrustedFromLoopback) {}

void MasterHttpServer::add(network::MasterRoute route,
                           network::ApiHandler handler) {
  server_.add(route.method, network::apiPathOf(route), std::move(handler));
}

std::uint16_t MasterHttpServer::open(const network::Endpoint& endpoint) {
  return server_.open(endpoint);
}

void MasterHttpServer::close() { server_.close(); }

}  // namespace rtype::master
