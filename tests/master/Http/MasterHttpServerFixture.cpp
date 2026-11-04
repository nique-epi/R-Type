#include "MasterHttpServerFixture.hpp"
#include <httplib.h>
#include <cstdint>
#include <string>
#include <string_view>
#include "HttpEndpoint.hpp"
#include "MasterHttpServer.hpp"
#include "MasterHttpServerTestConstants.hpp"
#include "NetworkConstants.hpp"

MasterHttpServerFixture::MasterHttpServerFixture()
    : server_([this]([[maybe_unused]] const std::string& reason) {
        ++failureCount_;
      }) {}

rtype::master::MasterHttpServer& MasterHttpServerFixture::server() {
  return server_;
}

void MasterHttpServerFixture::openServer() {
  port_ = server_.open(
      rtype::master::HttpEndpoint{.address = std::string{LOOPBACK_ADDRESS},
                                  .port = rtype::network::ANY_PORT});
}

httplib::Result MasterHttpServerFixture::get(std::string_view path) const {
  httplib::Client client{std::string{LOOPBACK_ADDRESS}, port_};
  return client.Get(std::string{path});
}

httplib::Result MasterHttpServerFixture::getWithBody(
    std::string_view path, const std::string& body) const {
  httplib::Request request;
  request.method = std::string{GET_METHOD};
  request.path = std::string{path};
  request.body = body;
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{TEXT_CONTENT_TYPE});
  httplib::Client client{std::string{LOOPBACK_ADDRESS}, port_};
  return client.send(request);
}

std::uint16_t MasterHttpServerFixture::port() const { return port_; }

int MasterHttpServerFixture::failureCount() const { return failureCount_; }
