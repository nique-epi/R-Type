#include "ClientConnectionModuleFixture.hpp"
#include <array>
#include <chrono>
#include <memory>
#include <optional>
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
#include "Endpoint.hpp"
#include "EntityAppearancesModule.hpp"
#include "HttpPortProbe.hpp"
#include "IModule.hpp"
#include "IServerCatalog.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "NetworkConditionsConstants.hpp"
#include "NetworkConstants.hpp"
#include "PlayerSettings.hpp"
#include "PlayerSettingsModule.hpp"
#include "RunningMaster.hpp"
#include "ServerCatalogFindingModule.hpp"
#include "ServerListPhase.hpp"
#include "SettingDefinitionConstants.hpp"

namespace rtype::client::testing {

void ClientConnectionModuleFixture::TearDown() {
  if (application_ != nullptr) {
    application_->stop();
  }
}

void ClientConnectionModuleFixture::startWith(const std::string& line,
                                              const PlayerSettings& settings) {
  const std::array<std::string_view, 1> quiet{logging::NO_STDERR_OPTION};
  auto settingsModule = std::make_unique<PlayerSettingsModule>(settings);
  settings_ = settingsModule.get();
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLoggerModule>(
      logOptionsOf(quiet, journal_.path())));
  modules.push_back(std::make_unique<EntityAppearancesModule>());
  modules.push_back(std::move(settingsModule));
  modules.push_back(
      std::make_unique<ClientConnectionModule>(network::PERFECT_NETWORK));
  modules.push_back(std::make_unique<ServerCatalogFindingModule>(catalog_));
  application_ = std::make_unique<engine::Application>(
      engine::LoopStages(CLIENT_LOOP_STAGES), std::move(modules),
      engine::configurationStep(folder_.path() / CLIENT_CONFIGURATION_FILE_NAME,
                                std::vector<std::string>{line}, report_));
  application_->start();
}

void ClientConnectionModuleFixture::askListing() { catalog_->refresh(); }

ServerListPhase ClientConnectionModuleFixture::phaseSoon(
    ServerListPhase awaited) {
  const auto deadline = std::chrono::steady_clock::now() + LISTING_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    application_->runStages(CONNECTION_FRAME);
    if (catalog_->snapshot().phase == awaited) {
      return awaited;
    }
    std::this_thread::sleep_for(LISTING_POLL_STEP);
  }
  return catalog_->snapshot().phase;
}

void ClientConnectionModuleFixture::chooseMaster(
    const network::Endpoint& master) {
  static_cast<void>(settings_->service().apply(
      MASTER_SETTING, std::optional<network::Endpoint>{master}));
}

network::Endpoint ClientConnectionModuleFixture::masterEndpoint() const {
  return network::Endpoint{
      .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
      .port = master_.port()};
}

std::string ClientConnectionModuleFixture::urlWithoutMaster() {
  return RunningMaster::urlOf(freeHttpPort());
}

}  // namespace rtype::client::testing
