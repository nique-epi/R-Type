#include "MasterModules.hpp"
#include <memory>
#include <string>
#include <vector>
#include "BuiltModuleConstants.hpp"
#include "ExecutablePath.hpp"
#include "FailureReport.hpp"
#include "IModule.hpp"
#include "LeaderboardModule.hpp"
#include "LogLaunchOptions.hpp"
#include "MasterHttpModule.hpp"
#include "MasterLoggerModule.hpp"
#include "MasterProbeModule.hpp"
#include "MasterStorageModule.hpp"
#include "ServersModule.hpp"
#if RTYPE_MODULE_ACCOUNTS
#include "AccountsModule.hpp"
#endif
#if RTYPE_MODULE_WALLET
#include "WalletModule.hpp"
#endif
#if RTYPE_MODULE_CASINO
#include "CasinoModule.hpp"
#endif

namespace rtype::master {

std::vector<std::unique_ptr<engine::IModule>> masterModules(
    const logging::LogLaunchOptions& logOptions, const FailureReport& onFailure,
    [[maybe_unused]] const std::string& passwordKeyText) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<MasterLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<MasterStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<MasterProbeModule>());
  modules.push_back(std::make_unique<MasterHttpModule>(onFailure));
  modules.push_back(std::make_unique<ServersModule>());
#if RTYPE_MODULE_ACCOUNTS
  modules.push_back(std::make_unique<AccountsModule>(passwordKeyText));
#endif
#if RTYPE_MODULE_WALLET
  modules.push_back(std::make_unique<WalletModule>());
#endif
  modules.push_back(std::make_unique<LeaderboardModule>());
#if RTYPE_MODULE_CASINO
  modules.push_back(std::make_unique<CasinoModule>());
#endif
  return modules;
}

}  // namespace rtype::master
