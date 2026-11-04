#include "MasterModules.hpp"
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "ExecutablePath.hpp"
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "MasterHttpModule.hpp"
#include "MasterLoggerModule.hpp"
#include "MasterStorageModule.hpp"
#include "ServerKeyOrder.hpp"
#include "ServerKeysModule.hpp"

namespace rtype::master {

std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions,
    const FailureReport& onFailure) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<MasterLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<MasterStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<ServerKeysModule>(std::nullopt));
  modules.push_back(std::make_unique<MasterHttpModule>(onFailure));
  return modules;
}

std::vector<std::unique_ptr<engine::IModule>> serverKeyModules(
    const logging::LogLaunchOptions& logOptions, ServerKeyOrder order) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<MasterLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<MasterStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<ServerKeysModule>(std::move(order)));
  return modules;
}

}  // namespace rtype::master
