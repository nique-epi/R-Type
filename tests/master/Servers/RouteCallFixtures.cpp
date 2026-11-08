#include "RouteCallFixtures.hpp"
#include <httplib.h>
#include <cstdint>
#include <string>
#include "HttpPortProbe.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"

httplib::Result sendRouteCall(std::uint16_t port, const RouteCall& call) {
  httplib::Request request;
  request.method = std::string{call.method};
  request.path = std::string{call.path};
  request.body = call.body;
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{EXPECTED_JSON_TYPE});
  if (!call.key.empty()) {
    request.set_header(std::string{AUTHORIZATION_HEADER_NAME},
                       std::string{BEARER_PREFIX_TEXT} + std::string{call.key});
  }
  return httpSend(port, request);
}
