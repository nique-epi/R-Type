#include "MasterModules.hpp"
#include <memory>
#include <vector>
#include "ExecutablePath.hpp"
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "MasterHttpModule.hpp"
#include "MasterLoggerModule.hpp"
#include "MasterStorageModule.hpp"
#include "ServersModule.hpp"

namespace rtype::master {

std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions,
    const FailureReport& onFailure) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<MasterLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<MasterStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<MasterHttpModule>(onFailure));
  modules.push_back(std::make_unique<ServersModule>());
  return modules;
}

}  // namespace rtype::master
