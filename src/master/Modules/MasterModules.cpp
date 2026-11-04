#include "MasterModules.hpp"
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ExecutablePath.hpp"
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "MasterHttpModule.hpp"
#include "MasterLoggerModule.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterStorageModule.hpp"
#include "ModuleSwitchesConstants.hpp"
#include "ServerKeyOrder.hpp"
#include "ServerKeysModule.hpp"
#include "ServersModule.hpp"

namespace rtype::master {

std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions, const FailureReport& onFailure,
    std::optional<ServerKeyOrder> order) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<MasterLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<MasterStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<ServerKeysModule>(std::move(order)));
  modules.push_back(std::make_unique<MasterHttpModule>(onFailure));
  modules.push_back(std::make_unique<ServersModule>());
  return modules;
}

std::vector<std::string> keyIssueLines() {
  std::vector<std::string> lines;
  for (const std::string_view name : {SERVERS_MODULE_NAME, HTTP_MODULE_NAME}) {
    lines.push_back(std::string{engine::MODULE_COMMAND_NAME} + " " +
                    std::string{name} + " " +
                    std::string{engine::SWITCH_OFF_WORD});
  }
  return lines;
}

}  // namespace rtype::master
