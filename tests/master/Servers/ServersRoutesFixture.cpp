#include "ServersRoutesFixture.hpp"
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "HttpPortProbe.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServerHeartbeat.hpp"
#include "ServersRoutesTestConstants.hpp"

ServersRoutesFixture::ServersRoutesFixture() {
  const httplib::Result created = createParis();
  if (created) {
    const nlohmann::json creation = nlohmann::json::parse(created->body);
    createdId_ = creation.at(ID_MEMBER_NAME).get<std::int64_t>();
    key_ = creation.at(KEY_MEMBER_NAME).get<std::string>();
  }
}

std::int64_t ServersRoutesFixture::createdId() const { return createdId_; }

const std::string& ServersRoutesFixture::key() const { return key_; }

httplib::Result ServersRoutesFixture::send(const RouteCall& call) const {
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
  return httpSend(master_.port(), request);
}

httplib::Result ServersRoutesFixture::createParis() const {
  return send(RouteCall{.method = POST_METHOD,
                        .path = SERVERS_URL,
                        .key = {},
                        .body = std::string{PARIS_CREATION_BODY}});
}

httplib::Result ServersRoutesFixture::sendHeartbeat(
    const rtype::network::ServerHeartbeat& heartbeat) const {
  return send(RouteCall{.method = PUT_METHOD,
                        .path = OWN_HEARTBEAT_URL,
                        .key = key_,
                        .body = rtype::network::heartbeatText(heartbeat)});
}

httplib::Result ServersRoutesFixture::readGames(
    std::string_view serverId) const {
  const std::string path = std::string{SERVER_GAMES_PATH_PREFIX} +
                           std::string{serverId} +
                           std::string{SERVER_GAMES_PATH_SUFFIX};
  return send(RouteCall{
      .method = GET_METHOD_TEXT, .path = path, .key = {}, .body = {}});
}

httplib::Result ServersRoutesFixture::registerParis() const {
  return send(RouteCall{.method = PUT_METHOD,
                        .path = OWN_SERVER_URL,
                        .key = key_,
                        .body = rtype::network::registrationText(
                            rtype::network::testing::parisRegistration())});
}
