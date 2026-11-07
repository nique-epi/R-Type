#include "ClientModules.hpp"
#include <memory>
#include <vector>
#include "AssetLoad.hpp"
#include "BuiltModuleConstants.hpp"
#include "ClientAssetsModule.hpp"
#include "ClientConnectionModule.hpp"
#include "ClientGameModule.hpp"
#include "ClientLobbyModule.hpp"
#include "ClientLoggerModule.hpp"
#include "ClientMenusModule.hpp"
#include "ClientMetricsModule.hpp"
#include "ClientScreensModule.hpp"
#include "ClientSettingsModule.hpp"
#include "ClientWindowModule.hpp"
#include "FrameLoop.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "ScreenAssets.hpp"
#include "SystemClock.hpp"
#if RTYPE_MODULE_AUDIO
#include "ClientAudioModule.hpp"
#endif

namespace rtype::client {

std::vector<std::unique_ptr<engine::IModule>> clientModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, FrameLoop& loop) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLoggerModule>(logOptions));
  modules.push_back(std::make_unique<ClientAssetsModule>(
      std::vector<AssetLoad>{loadInterfaceAssets, loadIntroAssets}));
  modules.push_back(std::make_unique<ClientSettingsModule>());
  modules.push_back(std::make_unique<ClientWindowModule>(loop));
  modules.push_back(std::make_unique<ClientConnectionModule>(
      launchOptions.networkConditions()));
  modules.push_back(std::make_unique<ClientMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
#if RTYPE_MODULE_AUDIO
  modules.push_back(
      std::make_unique<ClientAudioModule>(launchOptions.audioMode()));
#endif
  modules.push_back(
      std::make_unique<ClientScreensModule>(launchOptions.serverChoice()));
  modules.push_back(std::make_unique<ClientMenusModule>());
  modules.push_back(std::make_unique<ClientLobbyModule>());
  modules.push_back(std::make_unique<ClientGameModule>());
  return modules;
}

}  // namespace rtype::client
