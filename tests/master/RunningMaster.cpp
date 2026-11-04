#include "RunningMaster.hpp"
#include <array>
#include <cstdint>
#include <memory>
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
}

std::uint16_t RunningMaster::port() const { return port_; }

std::string RunningMaster::url() const {
  return std::string{HTTP_URL_PREFIX} + std::string{LOOPBACK_ADDRESS} +
         std::string{PORT_SEPARATOR} + std::to_string(port_);
}

void RunningMaster::stop() { application_->stop(); }
