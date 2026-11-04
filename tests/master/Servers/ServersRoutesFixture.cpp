#include "ServersRoutesFixture.hpp"
#include <httplib.h>
#include <array>
#include <chrono>
#include <memory>
#include <optional>
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
#include "MasterDatabase.hpp"
#include "MasterModules.hpp"
#include "RouteCall.hpp"
#include "ServerKeys.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "SettingLine.hpp"

ServersRoutesFixture::ServersRoutesFixture()
    : port_(freeHttpPort()), key_(issuedKey()) {
  const std::array<std::string_view, 1> quiet{rtype::logging::NO_STDERR_OPTION};
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::master::MASTER_LOOP_STAGES),
      rtype::master::masterModules(
          logOptionsOf(quiet, journal_.path()),
          []([[maybe_unused]] const std::string& reason) {}, std::nullopt),
      rtype::engine::configurationStep(
          folder_.path() / rtype::master::MASTER_CONFIGURATION_FILE_NAME,
          std::vector<std::string>{
              rtype::engine::settingLine(
                  rtype::master::DATABASE_SETTING_NAME,
                  (folder_.path() / ROUTES_DATABASE_FILE).string()),
              rtype::engine::settingLine(rtype::master::PORT_SETTING_NAME,
                                         std::to_string(port_))},
          report_));
  application_->start();
}

void ServersRoutesFixture::TearDown() { application_->stop(); }

const std::string& ServersRoutesFixture::key() const { return key_; }

httplib::Result ServersRoutesFixture::put(const RouteCall& call) const {
  httplib::Request request;
  request.method = std::string{PUT_METHOD_NAME};
  request.path = std::string{call.path};
  request.body = call.body;
  request.set_header(std::string{CONTENT_TYPE_NAME},
                     std::string{JSON_TYPE_NAME});
  if (!call.key.empty()) {
    request.set_header(std::string{AUTHORIZATION_NAME},
                       std::string{BEARER_PREFIX_TEXT} + std::string{call.key});
  }
  return httpSend(port_, request);
}

std::string ServersRoutesFixture::issuedKey() const {
  rtype::master::MasterDatabase database;
  database.open((folder_.path() / ROUTES_DATABASE_FILE).string());
  rtype::master::ServerKeys keys{database};
  std::string key = keys.issue(std::string{ROUTES_KEY_NAME},
                               std::chrono::floor<std::chrono::seconds>(
                                   std::chrono::system_clock::now()));
  database.close();
  return key;
}
