#include "SiteCreationRoutesFixtures.hpp"
#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "HttpPortProbe.hpp"
#include "MasterConstants.hpp"
#include "MasterTestConstants.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "SettingLine.hpp"
#include "SiteClaim.hpp"
#include "SiteClaimTestConstants.hpp"

std::vector<std::string> siteKeyLines() {
  return {rtype::engine::settingLine(rtype::master::SITE_KEY_SETTING_NAME,
                                     TEST_SITE_KEY)};
}

int siteCreationStatus(std::uint16_t port,
                       const rtype::master::SiteClaim& claim) {
  httplib::Request request;
  request.method = std::string{POST_METHOD};
  request.path = std::string{SERVERS_URL};
  request.body = std::string{PARIS_CREATION_BODY};
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{EXPECTED_JSON_TYPE});
  if (!claim.key.empty()) {
    request.set_header(std::string{SITE_KEY_HEADER_NAME}, claim.key);
  }
  if (!claim.clientAddress.empty()) {
    request.set_header(std::string{SITE_CLIENT_ADDRESS_HEADER_NAME},
                       claim.clientAddress);
  }
  const httplib::Result answer = httpSend(port, request);
  if (!answer) {
    return NO_ANSWER_STATUS;
  }
  return answer->status;
}

void createEveryServerOfAnHour(std::uint16_t port,
                               const rtype::master::SiteClaim& claim) {
  for (std::size_t creation = 0; creation < CREATIONS_PER_HOUR; ++creation) {
    ASSERT_EQ(siteCreationStatus(port, claim), EXPECTED_CREATED);
  }
}
