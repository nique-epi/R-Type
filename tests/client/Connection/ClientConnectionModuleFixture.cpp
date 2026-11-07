#include "ClientConnectionModuleFixture.hpp"
#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ClientConnectionModule.hpp"
#include "ClientConnectionModuleTestConstants.hpp"
#include "ClientConstants.hpp"
#include "ClientLoggerModule.hpp"
#include "ConfigurationReader.hpp"
#include "EntityAppearancesModule.hpp"
#include "IModule.hpp"
#include "IServerCatalog.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "NetworkConditionsConstants.hpp"
#include "RunningMaster.hpp"
#include "ServerCatalogFindingModule.hpp"
#include "ServerListPhase.hpp"

namespace rtype::client::testing {

void ClientConnectionModuleFixture::TearDown() {
  if (application_ != nullptr) {
    application_->stop();
  }
}

void ClientConnectionModuleFixture::startWith(const std::string& line) {
  const std::array<std::string_view, 1> quiet{logging::NO_STDERR_OPTION};
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLoggerModule>(
      logOptionsOf(quiet, journal_.path())));
  modules.push_back(std::make_unique<EntityAppearancesModule>());
  modules.push_back(
      std::make_unique<ClientConnectionModule>(network::PERFECT_NETWORK));
  modules.push_back(std::make_unique<ServerCatalogFindingModule>(catalog_));
  application_ = std::make_unique<engine::Application>(
      engine::LoopStages(CLIENT_LOOP_STAGES), std::move(modules),
      engine::configurationStep(folder_.path() / CLIENT_CONFIGURATION_FILE_NAME,
                                std::vector<std::string>{line}, report_));
  application_->start();
}

ServerListPhase ClientConnectionModuleFixture::listingPhaseSoon() {
  catalog_->refresh();
  const auto deadline = std::chrono::steady_clock::now() + LISTING_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    application_->runStages(CONNECTION_FRAME);
    if (catalog_->snapshot().phase != ServerListPhase::Searching) {
      return catalog_->snapshot().phase;
    }
    std::this_thread::sleep_for(LISTING_POLL_STEP);
  }
  return ServerListPhase::Searching;
}

const RunningMaster& ClientConnectionModuleFixture::master() const {
  return master_;
}

}  // namespace rtype::client::testing
