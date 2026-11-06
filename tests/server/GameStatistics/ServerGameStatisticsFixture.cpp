#include "ServerGameStatisticsFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReader.hpp"
#include "GameBusesModule.hpp"
#include "GameStatisticsFixtures.hpp"
#include "IModule.hpp"
#include "ListeningPortModule.hpp"
#include "LoopStages.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "PluginHostModule.hpp"
#include "RecordingJournal.hpp"
#include "ServerConstants.hpp"
#include "ServerGameStatisticsModuleTestConstants.hpp"
#include "SettingLine.hpp"
#include "TimeConstants.hpp"
#include "TrafficFixtures.hpp"

void ServerGameStatisticsFixture::start() {
  startWritingTo(folder_.path() / STATISTICS_FILE_NAME);
}

void ServerGameStatisticsFixture::startWritingTo(
    const std::filesystem::path& file) {
  startWith({rtype::engine::settingLine(STATISTICS_FILE_SETTING,
                                        rtype::logging::genericText(file))});
}

void ServerGameStatisticsFixture::startWithTheDefaultFile() { startWith({}); }

void ServerGameStatisticsFixture::runTicks(std::size_t ticks) const {
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    application_->runStages(rtype::engine::SIMULATION_TICK_DURATION);
  }
}

void ServerGameStatisticsFixture::stop() { application_->stop(); }

std::string ServerGameStatisticsFixture::fileText() const {
  return rtype::network::testing::textOf(folder_.path() / STATISTICS_FILE_NAME);
}

const RecordingJournal& ServerGameStatisticsFixture::journal() const {
  return host_.journal;
}

const std::filesystem::path& ServerGameStatisticsFixture::folder() const {
  return folder_.path();
}

void ServerGameStatisticsFixture::startWith(
    const std::vector<std::string>& lines) {
  std::vector<std::string> configuration = gameStatisticsPluginLines();
  configuration.insert(configuration.end(), lines.begin(), lines.end());
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<PluginHostModule>(
      std::string{rtype::server::LOGGER_MODULE_NAME}, host_));
  modules.push_back(
      std::make_unique<ListeningPortModule>(STATISTICS_TEST_PORT));
  auto games = std::make_unique<GameBusesModule>(GAMES_IN_THE_TESTS);
  games_ = games.get();
  modules.push_back(std::move(games));
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules),
      rtype::engine::configurationStep(
          folder_.path() / rtype::server::SERVER_CONFIGURATION_FILE_NAME,
          std::move(configuration), report_));
  application_->start();
  EXPECT_EQ(report_.refusals, std::vector<std::string>{});
}
