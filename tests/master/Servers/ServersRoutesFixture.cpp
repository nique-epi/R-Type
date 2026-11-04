#include "ServersRoutesFixture.hpp"
#include <httplib.h>
#include <array>
#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "HttpPortProbe.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "MasterConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterModules.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "SettingLine.hpp"

ServersRoutesFixture::ServersRoutesFixture() : port_(freeHttpPort()) {
  databaseFile_.close();
  const std::array<std::string_view, 1> quiet{rtype::logging::NO_STDERR_OPTION};
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::master::MASTER_LOOP_STAGES),
      rtype::master::masterModules(
          logOptionsOf(quiet, journal_.path()),
          []([[maybe_unused]] const std::string& reason) {}),
      rtype::engine::configurationStep(
          databaseFile_.file().parent_path() /
              rtype::master::MASTER_CONFIGURATION_FILE_NAME,
          std::vector<std::string>{
              rtype::engine::settingLine(rtype::master::DATABASE_SETTING_NAME,
                                         databaseFile_.file().string()),
              rtype::engine::settingLine(rtype::master::PORT_SETTING_NAME,
                                         std::to_string(port_))},
          report_));
  application_->start();
  const httplib::Result created = createParis();
  if (created) {
    const nlohmann::json creation = nlohmann::json::parse(created->body);
    createdId_ = creation.at(ID_MEMBER_NAME).get<std::int64_t>();
    key_ = creation.at(KEY_MEMBER_NAME).get<std::string>();
  }
}

void ServersRoutesFixture::TearDown() { application_->stop(); }

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
  return httpSend(port_, request);
}

httplib::Result ServersRoutesFixture::createParis() const {
  return send(RouteCall{.method = POST_METHOD,
                        .path = SERVERS_URL,
                        .key = {},
                        .body = std::string{PARIS_CREATION_BODY}});
}

httplib::Result ServersRoutesFixture::registerParis() const {
  return send(RouteCall{.method = PUT_METHOD,
                        .path = OWN_SERVER_URL,
                        .key = key_,
                        .body = rtype::network::registrationText(
                            rtype::network::testing::parisRegistration())});
}
