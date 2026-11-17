#include "MasterHttpServer.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include "ApiHandler.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "Endpoint.hpp"
#include "FailureReport.hpp"
#include "ForwardedAddress.hpp"
#include "HttpConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterRoute.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkConstants.hpp"

namespace rtype::master {

namespace {

/** @returns @p handler, behind a check that answers 403 to a request that
 * did not reach the master over HTTPS. */
network::ApiHandler httpsOnly(network::ApiHandler handler) {
  return [answer = std::move(handler)](const network::ApiRequest& request) {
    if (!request.overHttps) {
      return network::ApiResponse{.status = FORBIDDEN_STATUS,
                                  .body = {},
                                  .detail = std::string{HTTPS_REQUIRED_DETAIL}};
    }
    return answer(request);
  };
}

}  // namespace

MasterHttpServer::MasterHttpServer(FailureReport onFailure)
    : server_(std::string{MASTER_LOGGER_NAME}, std::move(onFailure),
              network::ForwardedAddress::TrustedFromLoopback) {}

void MasterHttpServer::add(network::MasterRoute route,
                           network::ApiHandler handler) {
  server_.add(
      route.method, network::apiPathOf(route),
      route.httpsOnly ? httpsOnly(std::move(handler)) : std::move(handler));
}

std::uint16_t MasterHttpServer::open(const network::Endpoint& endpoint) {
  port_ = server_.open(endpoint);
  return port_;
}

void MasterHttpServer::close() {
  server_.close();
  port_ = network::ANY_PORT;
}

std::uint16_t MasterHttpServer::port() const { return port_; }

}  // namespace rtype::master
