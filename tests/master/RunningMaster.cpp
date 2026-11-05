#include "RunningMaster.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <array>
#include <cstdint>
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
#include "MasterModules.hpp"
#include "MasterTestConstants.hpp"
#include "SettingLine.hpp"

RunningMaster::RunningMaster() : port_(freeHttpPort()) {
  databaseFile_.close();
  application_ = application();
  application_->start();
}

std::unique_ptr<rtype::engine::Application> RunningMaster::application() {
  const std::array<std::string_view, 1> quiet{rtype::logging::NO_STDERR_OPTION};
  return std::make_unique<rtype::engine::Application>(
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
}

std::uint16_t RunningMaster::port() const { return port_; }

std::string RunningMaster::url() const { return urlOf(port_); }

std::string RunningMaster::urlOf(std::uint16_t port) {
  return std::string{HTTP_URL_PREFIX} + std::string{LOOPBACK_ADDRESS} +
         std::string{PORT_SEPARATOR} + std::to_string(port);
}

void RunningMaster::stop() { application_.reset(); }

void RunningMaster::restart() {
  application_.reset();
  application_ = application();
  application_->start();
}

std::optional<std::string> RunningMaster::lastHeartbeatOf(
    std::int64_t serverId) const {
  const SQLite::Database reader{databaseFile_.file().string()};
  SQLite::Statement query{reader, std::string{LAST_HEARTBEAT_OF_SERVER_QUERY}};
  query.bind(std::string{SERVER_ID_PARAMETER_NAME}, serverId);
  if (!query.executeStep() || query.getColumn(LAST_HEARTBEAT_COLUMN).isNull()) {
    return std::nullopt;
  }
  return query.getColumn(LAST_HEARTBEAT_COLUMN).getString();
}
