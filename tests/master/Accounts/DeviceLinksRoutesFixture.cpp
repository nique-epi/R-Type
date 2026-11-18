#include "DeviceLinksRoutesFixture.hpp"
#include <httplib.h>
#include <string>
#include <string_view>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "DeviceLinksRoutesTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "PlayerLinkCall.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "RunningMaster.hpp"
#include "ServersRoutesTestConstants.hpp"

const RunningMaster& DeviceLinksRoutesFixture::master() const {
  return master_;
}

httplib::Result DeviceLinksRoutesFixture::send(const RouteCall& call) const {
  return sendRouteCall(master_.port(), call);
}

httplib::Result DeviceLinksRoutesFixture::createLink() const {
  return send(RouteCall{.method = POST_METHOD,
                        .path = DEVICE_LINKS_URL,
                        .key = {},
                        .body = {},
                        .overHttps = true});
}

std::string DeviceLinksRoutesFixture::aceToken() const {
  static_cast<void>(send(credentialsCall(USERS_URL, aceCredentials())));
  return memberIn(send(credentialsCall(SESSIONS_URL, aceCredentials())),
                  TOKEN_MEMBER_NAME);
}

httplib::Result DeviceLinksRoutesFixture::readOwnLink(
    std::string_view secret) const {
  return send(RouteCall{.method = GET_METHOD_TEXT,
                        .path = OWN_DEVICE_LINK_URL,
                        .key = secret,
                        .body = {},
                        .overHttps = true});
}

httplib::Result DeviceLinksRoutesFixture::tradeLink(
    std::string_view secret) const {
  return send(RouteCall{.method = POST_METHOD,
                        .path = SESSIONS_URL,
                        .key = secret,
                        .body = {},
                        .overHttps = true});
}

httplib::Result DeviceLinksRoutesFixture::forgetLink(
    std::string_view secret) const {
  return send(RouteCall{.method = DELETE_METHOD_TEXT,
                        .path = OWN_DEVICE_LINK_URL,
                        .key = secret,
                        .body = {},
                        .overHttps = true});
}

httplib::Result DeviceLinksRoutesFixture::readLink(
    const PlayerLinkCall& call) const {
  const std::string path =
      std::string{DEVICE_LINK_URL_START}.append(call.userCode);
  return send(RouteCall{.method = GET_METHOD_TEXT,
                        .path = path,
                        .key = call.token,
                        .body = {},
                        .overHttps = true});
}

httplib::Result DeviceLinksRoutesFixture::decide(
    const PlayerLinkCall& call) const {
  const std::string path = std::string{DEVICE_LINK_URL_START}
                               .append(call.userCode)
                               .append(DECISION_URL_END);
  return send(RouteCall{.method = PUT_METHOD,
                        .path = path,
                        .key = call.token,
                        .body = std::string{call.body},
                        .overHttps = true});
}
