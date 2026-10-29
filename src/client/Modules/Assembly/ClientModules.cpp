#include "ClientModules.hpp"
#include <memory>
#include <vector>
#include "ClientAssetsModule.hpp"
#include "ClientConnectionModule.hpp"
#include "ClientLoggerModule.hpp"
#include "ClientScreensModule.hpp"
#include "ClientSettingsModule.hpp"
#include "ClientWindowModule.hpp"
#include "FrameLoop.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"

namespace rtype::client {

std::vector<std::unique_ptr<engine::IModule>> clientModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, FrameLoop& loop) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientLoggerModule>(
      logOptions, launchOptions.serverEndpoint()));
  modules.push_back(std::make_unique<ClientAssetsModule>());
  modules.push_back(std::make_unique<ClientSettingsModule>());
  modules.push_back(std::make_unique<ClientWindowModule>(loop));
  modules.push_back(std::make_unique<ClientConnectionModule>());
  modules.push_back(std::make_unique<ClientScreensModule>(
      launchOptions.serverEndpoint(), launchOptions.nickname()));
  return modules;
}

}  // namespace rtype::client
