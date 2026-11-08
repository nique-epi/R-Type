#include "ServersRoutesFixture.hpp"
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "ServerHeartbeat.hpp"
#include "ServersRoutesTestConstants.hpp"

namespace {

/** @returns The registration of "Paris 1", sent with @p key. */
RouteCall parisRegistrationCall(std::string_view key) {
  return RouteCall{.method = PUT_METHOD,
                   .path = OWN_SERVER_URL,
                   .key = key,
                   .body = rtype::network::registrationText(
                       rtype::network::testing::parisRegistration())};
}

}  // namespace

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
  return sendRouteCall(master_.port(), call);
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
  return readServerPart(serverId, SERVER_GAMES_PATH_SUFFIX);
}

httplib::Result ServersRoutesFixture::readServers(
    std::string_view query) const {
  const std::string path = std::string{SERVERS_URL} + std::string{query};
  return send(RouteCall{
      .method = GET_METHOD_TEXT, .path = path, .key = {}, .body = {}});
}

httplib::Result ServersRoutesFixture::readServerPart(
    std::string_view serverId, std::string_view part) const {
  const std::string path = std::string{SERVER_GAMES_PATH_PREFIX} +
                           std::string{serverId} + std::string{part};
  return send(RouteCall{
      .method = GET_METHOD_TEXT, .path = path, .key = {}, .body = {}});
}

httplib::Result ServersRoutesFixture::registerParis() const {
  return send(parisRegistrationCall(key_));
}

std::optional<std::int64_t> ServersRoutesFixture::registerSecondServer() const {
  const httplib::Result created = createParis();
  if (!created) {
    return std::nullopt;
  }
  const nlohmann::json creation = nlohmann::json::parse(created->body);
  const std::string secondKey = creation.at(KEY_MEMBER_NAME).get<std::string>();
  if (!send(parisRegistrationCall(secondKey))) {
    return std::nullopt;
  }
  return creation.at(ID_MEMBER_NAME).get<std::int64_t>();
}
