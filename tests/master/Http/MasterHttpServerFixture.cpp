#include "MasterHttpServerFixture.hpp"
#include <httplib.h>
#include <cstdint>
#include <string>
#include <string_view>
#include "ApiResponse.hpp"
#include "Endpoint.hpp"
#include "HttpPortProbe.hpp"
#include "MasterHttpServer.hpp"
#include "MasterHttpServerTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkConstants.hpp"

MasterHttpServerFixture::MasterHttpServerFixture()
    : server_([this]([[maybe_unused]] const std::string& reason) {
        ++failureCount_;
      }) {}

rtype::master::MasterHttpServer& MasterHttpServerFixture::server() {
  return server_;
}

void MasterHttpServerFixture::addTestRoute() {
  server_.addGet(TEST_ROUTE, [] {
    return rtype::master::ApiResponse{.status = EXPECTED_OK,
                                      .body = std::string{TEST_ROUTE_BODY}};
  });
}

void MasterHttpServerFixture::openServer() {
  port_ = server_.open(rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = rtype::network::ANY_PORT});
}

httplib::Result MasterHttpServerFixture::get(std::string_view path) const {
  return httpGet(port_, path);
}

httplib::Result MasterHttpServerFixture::getWithBody(
    std::string_view path, const std::string& body) const {
  httplib::Request request;
  request.method = std::string{GET_METHOD};
  request.path = std::string{path};
  request.body = body;
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{TEXT_CONTENT_TYPE});
  return httpSend(port_, request);
}

std::uint16_t MasterHttpServerFixture::port() const { return port_; }

int MasterHttpServerFixture::failureCount() const { return failureCount_; }
