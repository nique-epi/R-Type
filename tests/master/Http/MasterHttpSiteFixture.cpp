#include "MasterHttpSiteFixture.hpp"
#include <httplib.h>
#include <filesystem>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "Endpoint.hpp"
#include "HttpPortProbe.hpp"
#include "MasterHttpServerTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "NetworkConstants.hpp"

MasterHttpSiteFixture::MasterHttpSiteFixture() : server_(quietHttpServer()) {}

void MasterHttpSiteFixture::writeSiteFile(std::string_view file,
                                          std::string_view content) const {
  folder_.write(std::filesystem::path{SITE_FOLDER} / file, content);
}

void MasterHttpSiteFixture::writeOutsideFile(std::string_view file,
                                             std::string_view content) const {
  folder_.write(std::filesystem::path{file}, content);
}

void MasterHttpSiteFixture::addTestRoute() {
  server_.add(TEST_GET_ROUTE,
              []([[maybe_unused]] const rtype::master::ApiRequest& request) {
                return rtype::master::ApiResponse{
                    .status = EXPECTED_OK,
                    .body = std::string{TEST_ROUTE_BODY}};
              });
}

void MasterHttpSiteFixture::serveSite(std::string_view folder) {
  site_ = folder_.path() / folder;
  server_.serveSite([this] { return site_; });
}

void MasterHttpSiteFixture::openServer() {
  port_ = server_.open(rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = rtype::network::ANY_PORT});
}

httplib::Result MasterHttpSiteFixture::get(std::string_view path) const {
  return httpGet(port_, path);
}
