#include "MasterModulesFixture.hpp"
#include <httplib.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "HttpPortProbe.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "MasterConstants.hpp"
#include "MasterModules.hpp"
#include "MasterModulesTestConstants.hpp"
#include "SettingLine.hpp"

MasterModulesFixture::MasterModulesFixture()
    : journalPath_(journal_.path().string()),
      journalArguments_{rtype::logging::FILE_OPTION, journalPath_,
                        rtype::logging::NO_STDERR_OPTION},
      port_(freeHttpPort()) {}

std::unique_ptr<rtype::engine::Application>
MasterModulesFixture::masterApplication(
    std::vector<std::unique_ptr<rtype::engine::IModule>> extra,
    std::vector<std::string> lines) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules =
      rtype::master::masterModules(
          journalOptions(), []([[maybe_unused]] const std::string& reason) {});
  for (std::unique_ptr<rtype::engine::IModule>& module : extra) {
    modules.push_back(std::move(module));
  }
  std::vector<std::string> configuration{
      databaseLine(folder() / TEST_DATABASE_FILE),
      rtype::engine::settingLine(rtype::master::PORT_SETTING_NAME,
                                 std::to_string(port_))};
  configuration.insert(configuration.end(), lines.begin(), lines.end());
  return std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::master::MASTER_LOOP_STAGES),
      std::move(modules),
      rtype::engine::configurationStep(
          folder() / rtype::master::MASTER_CONFIGURATION_FILE_NAME,
          std::move(configuration), report_));
}

std::string MasterModulesFixture::databaseLine(
    const std::filesystem::path& file) {
  return rtype::engine::settingLine(rtype::master::DATABASE_SETTING_NAME,
                                    file.string());
}

httplib::Result MasterModulesFixture::get(std::string_view path) const {
  httplib::Client client{std::string{MASTER_TEST_ADDRESS}, port_};
  return client.Get(std::string{path});
}

const std::filesystem::path& MasterModulesFixture::folder() const {
  return folder_.path();
}

std::uint16_t MasterModulesFixture::port() const { return port_; }

rtype::logging::LogLaunchOptions MasterModulesFixture::journalOptions() const {
  return logOptionsOf(journalArguments_, rtype::logging::MASTER_LOG_FILE_NAME);
}
